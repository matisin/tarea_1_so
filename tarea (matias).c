#include <stdio.h>


#include <stdlib.h>

#include <string.h>

#define   buffer_n 100

void parser(char s2[]){
	char *s1 = malloc(250);
	char *copia;


	printf("El string original es: %s\n", s2);


	char delim[]=" \t";
	char *token;
	copia = strdup(s2);
	token = strtok(s2,delim);
	printf("%s\n", token);

	while(token = strtok(NULL, delim))
	{
		printf("%s\n", token);
	}
}

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}

int main (int argc, char *argv[]) {

	char *input;
	input = (char *) malloc(buffer_n*sizeof(char));

	system("clear");

<<<<<<< HEAD
=======

>>>>>>> 2bac749c2bdabfa9b72f69233d489e33b0cae8ca

	while(1) {

		printPrompt();//imprime el 


		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada
		parser(input); //aqui ingresamos el comando


		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}

	}

	free(input);

	printf("Donoso culiao\n");
}



