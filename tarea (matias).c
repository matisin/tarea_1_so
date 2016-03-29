#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define   buffer_n 100

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}

int main (int argc, char *argv[]) {

	char *input;
	input = (char *) malloc(buffer_n*sizeof(char));

	while(1) {


		printPrompt();//imprime el 

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { // se compara el primer caracter primero
			continue;
		}

	}
	free(input);
}