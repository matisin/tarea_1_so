#include <stdio.h>


#include <stdlib.h>

#include <string.h>

#define   buffer_n 100

char **parser(char s2[]){
	char *copia;
	char **tokens = malloc(buffer_n*sizeof(char));
	printf("El string original es: %s\n", s2);
	char delim[]=" \t";
	char *token;
	copia = strdup(s2);
	token = strtok(s2,delim);
	printf("%s\n", token);
	*(tokens) =  token;
	int i = 1;
	while(token = strtok(NULL, delim))
	{
		printf("%s\n", token);
		*(tokens+i) =  token;
		i++;
	}
	return tokens;
}

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}

int main (int argc, char *argv[]) {

	char *input,**token;
	input = (char *) malloc(buffer_n*sizeof(char));
	token = (char **) malloc(buffer_n*sizeof(char));
	int hay_pipe ; 
	int i;

	system("clear");

	while(1) {
		hay_pipe = 0;

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}

		token=parser(input); //aqui ingresamos el comando
		printf("%s\n",*(token) );

		for(i = 0; i < buffer_n ; i++ ){
			if(*(input+i)=='|'){//recoremos el input
				hay_pipe = 1; //si es comando con pipe
				break;
			}
		}
		if(hay_pipe){
			
		}else{
			
		}

	}

	free(input);
	free(token);

	printf("Donoso culiao\n");
}



