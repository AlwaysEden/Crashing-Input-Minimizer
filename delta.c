#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>


#ifdef DEBUG
	#define debug(fn) fn
#else
	#define debug(fn) 
#endif

#define MAX_SIZE_SEGMENT 4096


void alrm_handler(){
  if(kill(getpid(), 9) == -1){
    fprintf(stderr, "ERROR: kill child process\n");
  }
}

struct return_value{
	unsigned char *result;
	size_t result_size;
};

struct return_value* reduce(unsigned char *reduced_input, char* exec_file, char* detect_msg, size_t size){
	int child_parent_fd1[2];
	int parent_child_fd1[2];
	int child_parent_fd2[2];
	int parent_child_fd2[2];
	
	//fprintf(stderr, "%s %s %s %ld\n", reduced_input, exec_file, detect_msg, size);		
	char exec_file_cp[255];
	strcpy(exec_file_cp, exec_file);	
	char *token[50];
	char *tmp = strtok(exec_file_cp, " ");
	
	int i = 0;
	while(tmp != NULL){
		token[i] = strdup(tmp);
		i++;
		tmp = strtok(NULL, " ");
	}

	unsigned char input[MAX_SIZE_SEGMENT];
	strcpy(input, reduced_input);
	size_t input_size= size;	
	size_t s = input_size - 1;
	
	unsigned char head[MAX_SIZE_SEGMENT] = "";
	unsigned char tail[MAX_SIZE_SEGMENT] = "";
	unsigned char mid[MAX_SIZE_SEGMENT] = "";

	while(s > 0){
		fprintf(stderr,"s: %ld\n",s);
		for(int i = 0; i <= input_size - s; i++){
			memcpy(head, input, i);
			debug(fprintf(stderr,"head: %s\n", head);)
			if( !((i+s) > input_size-1) ){
				memcpy(tail, input+(i+s), (input_size-(i+s)));
			}
			debug(fprintf(stderr,"tail: %s\n", tail);)
			unsigned char result[MAX_SIZE_SEGMENT];
			size_t length = i + (input_size - (i+s));
			memcpy(result, strcat(head, tail), length);

			pipe(child_parent_fd1);
			pipe(parent_child_fd1);
			int exec_pid = fork();
			if(exec_pid == 0){
        			close(child_parent_fd1[0]);
        			close(parent_child_fd1[1]);
        			dup2(parent_child_fd1[0], STDIN_FILENO);
        			close(parent_child_fd1[0]);
        			dup2(child_parent_fd1[1], STDERR_FILENO);
        			close(child_parent_fd1[1]);
				alarm(3);
 				execvp(token[0],token);
				perror("execl");
			}else if(exec_pid > 0){
				close(parent_child_fd1[0]);
				close(child_parent_fd1[1]);
				write(parent_child_fd1[1], result, length); 
				close(parent_child_fd1[1]);
				int status;
				waitpid(exec_pid, &status, 0);

			}
			FILE *pipe_stream = fdopen(child_parent_fd1[0],"r");
			char *line = NULL;
			size_t len = 0;
			ssize_t nread;
			while( (nread = getline(&line, &len,pipe_stream)) != -1){
                                debug(fprintf(stderr, "line: %s\n", line);)
				if(strstr(line, detect_msg) != NULL){
					fprintf(stderr,"crash line s: %ld\n", s);
					debug(fprintf(stderr,"crash line: %s\n",result);)
					close(child_parent_fd1[0]);
					return reduce(result, exec_file, detect_msg, length);
				}
			}	
			close(child_parent_fd1[0]);
			memset(head, 0, sizeof(head));
			memset(tail, 0, sizeof(tail));	
			memset(result, 0, sizeof(result));
		}
		for(int i = 0; i <= input_size - s; i++){
			memcpy(mid, input+i, s);
			debug(fprintf(stderr,"mid: %s\n", mid);)

			pipe(child_parent_fd2);
			pipe(parent_child_fd2);
			int pid = fork();
			if(pid == 0){
        			close(child_parent_fd2[0]);
        			close(parent_child_fd2[1]);
        			dup2(parent_child_fd2[0], STDIN_FILENO);
        			dup2(child_parent_fd2[1], STDERR_FILENO);
        			close(parent_child_fd2[0]);
        			close(child_parent_fd2[1]);
				alarm(3);
 				execvp(token[0],token);
				perror("execl");
			}else if(pid > 0){
				close(parent_child_fd2[0]);
				close(child_parent_fd2[1]);
				write(parent_child_fd2[1], mid, s);
				close(parent_child_fd2[1]);
				int status;
				waitpid(pid, &status, 0);
			}
			FILE *pipe_stream = fdopen(child_parent_fd1[0],"r");
			char *line = NULL;
			size_t len = 0;
			ssize_t nread;
			while( (nread = getline(&line, &len,pipe_stream)) != -1){
				debug(fprintf(stderr, "line: %s\n", line);)
				if(strstr(line, detect_msg) != NULL){
					fprintf(stderr,"crash line s: %ld\n", s);
					debug(fprintf(stderr,"crash line: %s\n",mid);)
					close(child_parent_fd2[0]);
					return reduce(mid, exec_file, detect_msg, s);
				}
			}
			close(child_parent_fd2[0]);
			memset(mid, 0, sizeof(mid));
		}
		s -= 1;
	}
	struct return_value *return_input = (struct return_value *)malloc(sizeof(struct return_value));

	return_input->result = (unsigned char *)malloc(sizeof(unsigned char)*input_size);	
	return_input->result_size = input_size;
	memcpy(return_input->result, input, input_size);
	return return_input;
}


int main(int argc, char *argv[]){
	//signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, alrm_handler);
	int opt;
	unsigned char input[MAX_SIZE_SEGMENT];
	size_t input_size;
	struct stat sb;
	unsigned char detect_msg[1024];
	unsigned char result_filename[256];
	unsigned char exec_filename[256];
	FILE *file;

	while( (opt = getopt(argc, argv, "i:m:o:")) != -1 ){
		switch(opt){
			case 'i':
				file = fopen(optarg,"rb");
				if(stat(optarg, &sb) == -1){
					perror("stat: ");
					exit(1);
				}
				input_size = sb.st_size;
				read(fileno(file), input, MAX_SIZE_SEGMENT);
				fclose(file);	
				break;
			case 'm':
				strcpy(detect_msg, optarg);
				break;
			case 'o':
				strcpy(result_filename, optarg);
				break;
		}
	}
	for (int i = optind; i < argc; i++) {
		if(strstr(argv[i], "./") != NULL){
			strcpy(exec_filename, argv[i]);
		}
 	}

	struct return_value *result_val = reduce(input, exec_filename, detect_msg, input_size);
	FILE *fd = fopen(result_filename, "w+");
	//debug(fprintf(stderr,"before fwrite: %s %ld", result_val->result, result_val->result_size);)
	fwrite(result_val->result, 1, result_val->result_size, fd);
	free(result_val);
}
