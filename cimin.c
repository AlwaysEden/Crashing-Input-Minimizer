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

  // while(extract_length > 0) {
  //   for(int i=0; i<sm_length - extract_length; i++){
  //     if(i == 0){
  //       ptr = sm + 0;
  //       length = i - 0;
  //       head = strncpy(sm, ptr, length);
  //     }
  //     ptr = sm + i + extract_length;
  //     length = extract_length - (i + extract_length);
  //     tail = strncpy(sm, ptr, length);
  //     heil = strcat(head, tail);

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
    pipes[WRITE] = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC);
		int fd = open(argv[1], O_RDONLY);
    dup2(pipes[WRITE], STDOUT_FILENO);
		dup2(fd, STDIN_FILENO);
		close(fd);
    close(pipes[WRITE]);
    execl(argv[2],"<", argv[1], NULL); // Execute the target program
  } else { // parent process
		
    close(pipes[WRITE]);
		pipes[READ] = open("output.txt", O_RDONLY);
    char buffer[1024];
    if(read(pipes[READ], buffer, sizeof(buffer)) > 0){
			printf("%s", buffer);
			printf("제발,,\n");
		}else{
			printf("아닌가벼");
		}
    wait(NULL);
	}
  close(pipes[READ]);

  //***** Check if the error message is maintained
}



int main(int argc, char *argv[]){
  // if (argc != 2) { //이 부분은 나중에 구현할 예정. input arguments들이 올바른지, 존재유무 등을 판별해서 에러값을 띄우는 파트.
  //   printf("Usage: %s [input file]\n", argv[0]);
  //   return 1;
  // }
  FILE *file = freopen(argv[1], "rb", stdin); // 입력 파일을 열어 stdin으로 리디렉션
  if (!file) {
      printf("Failed to open input file %s\n", argv[1]);
      return 1;
  }
  if(pipe(pipes) != 0) {
    perror("pipe");
    exit(1);
  }

  fseek(file, 0, SEEK_END); // 파일 포인터를 파일 끝으로 이동시킴
  long length = ftell(file); // 파일의 길이를 계산함
  rewind(file); // Move File pointer to the beginning of the file

  char buffer[4096];
  fgets(buffer, sizeof(buffer), stdin); // buffer is made to use strcat in while loop
  char input[length];
  strcpy(input, buffer); // To use strcat, Input is filled with buffer. Because Error is occurred on empty array.
  while (fgets(buffer, sizeof(buffer), stdin)) {
    strcat(input, buffer);
  }

  reduce(input, argv);

  fclose(file); // 입력 파일 닫기
  return 0;
}


