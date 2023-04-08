#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int READ = 0;
int WRITE = 1;
int pipes[2];

void reduce(char s[], char *argv[]) { //The function to reduce the input with the error message
  char *sm = (char*)malloc(sizeof(char) * (strlen(s) + 1));
  strcpy(sm, s);
  sm[strlen(s)] = '\n';
  int sm_length = strlen(sm);
  int extract_length = sm_length - 1;
  char * head = "";
  char * tail = "";
  char * heil = ""; //Heap + tail
  char *ptr;
  int length = 0;
  pid_t pid;

  while(extract_length > 0) {
     for(int i=0; i<sm_length - extract_length; i++){
       if(i == 0){
         ptr = sm + 0;
         length = i - 0;
         head = strncpy(sm, ptr, length);
       }
	ptr = sm + i + extract_length;
	length = extract_length - (i + extract_length);
	tail = strncpy(sm, ptr, length);
	heil = strcat(head, tail);

  //***** Execute the target program *****
	if(pipe(pipes) != 0) {
        perror("pipe");
        exit(1);
  	}
	pid = fork();
	if (pid < 0) {
		perror("fork error");
		exit(1);
  	} else if (pid == 0) { // Child process
    		close(pipes[READ]); 
    		pipes[WRITE] = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // open the file to write
		int fd = open(argv[1], O_RDONLY); //open the file to read as test input
    		dup2(pipes[WRITE], STDOUT_FILENO); // pipes
		dup2(fd, STDIN_FILENO);
		close(fd);
    		close(pipes[WRITE]);
    		execl(argv[2], argv[1], NULL); // Execute the target program
  	} else { // parent process
		
    		close(pipes[WRITE]);
		pipes[READ] = open("output.txt", O_RDONLY);
    		char buffer[1024];
    		while(read(pipes[READ], buffer, sizeof(buffer)) > 0){
			printf("%s", buffer);
		}
    		wait(NULL);
	}
  close(pipes[READ]);

  //***** Check if the error message is maintained
	}
	}
}



int main(int argc, char *argv[]){
   if (argc < 2) { //Check if input argument is wrong.
     printf("Usage: %s [input file]\n", argv[0]);
     return 1;
   }
  FILE *file = freopen(argv[1], "rb", stdin); // Open input file and redirect to stdin
  if (!file) {
      printf("Failed to open input file %s\n", argv[1]);
      return 1;
  }
  if(pipe(pipes) != 0) {
    perror("pipe");
    exit(1);
  }

  fseek(file, 0, SEEK_END); // Move File pointer to the end of the file
  long length = ftell(file); // the length of file
  rewind(file); // Move File pointer to the beginning of the file

  char buffer[4096];
  fgets(buffer, sizeof(buffer), stdin); // buffer is made to use strcat in while loop
  char input[length];
  strcpy(input, buffer); // To use strcat, Input is filled with buffer. Because Error is occurred on empty array.
  while (fgets(buffer, sizeof(buffer), stdin)) { //read buffer and store to input
    strcat(input, buffer);
  }

  reduce(input, argv); //call reduce()

  fclose(file); // close input file
  return 0;
}


