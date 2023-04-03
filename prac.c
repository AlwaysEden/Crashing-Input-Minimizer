#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	execl(argv[2], argv[1], NULL);
	
}
