#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int READ = 0;
int WRITE = 1;
int pipes[2];

void reduce(char s[], char *argv[]) {
  char *sm = (char*)malloc(sizeof(char) * (strlen(s) + 1)); // 메모리 할당
  strcpy(sm, s); // 값 복사
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

      pid = fork();
      if (pid < 0) {
        perror("fork error");
        exit(1);
      } else if (pid == 0) { // 자식 프로세스
          printf("여기 안와?1");
          close(pipes[READ]); // 읽기용 파일 디스크립터 닫기
          dup2(pipes[WRITE], STDOUT_FILENO); // 표준 출력을 파이프에 연결
          close(pipes[WRITE]); // 원래 파일 디스크립터 닫기
          printf("여기 안와?");
	  int fd = open(argv[1],O_RDONLY);
	  dup2(fd, STDIN_FILENO);
	  close(fd);
          execl(argv[2],"<", argv[1], NULL); // 실행하고자 하는 프로그램 실행
      } else { // 부모 프로세스
          printf("여기 안와?2");
          close(pipes[WRITE]); // 쓰기용 파일 디스크립터 닫기
	  wait(NULL);
          char buffer[1024];
	  int total_read = 0;
	  while(total_read < sizeof(buffer)){
		int m = read(pipes[0], buffer+total_read, sizeof(buffer)-total_read); // 파이프로부터 읽기
		if (m = 0) break;
		total_read += m;
	  }
	  close(pipes[READ]);
          printf("Received: %.*s\n", total_read, buffer); // 받은 결과 출력
	}
  free(sm); // 할당된 메모리 해제
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


