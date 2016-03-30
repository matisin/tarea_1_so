#include <stdio.h>


#include <stdlib.h>

#include <string.h>

#define   buffer_n 1000


// parsea un string de entrada
//
// return char **

char **parser(char string[]){	
	char **tokens = malloc(buffer_n*sizeof(char));	
	char *delim = " \t\n";
	char *token =  strtok(string,delim);	
	int i = 0;
	while(token){			
		*(tokens+i) =  token;
		token = strtok(NULL, delim);
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
		hay_pipe = 0;//se asume que no hay pipe al principio

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}

		token=parser(input); //se guardan los tokens

		for(i = 0 ;i < buffer_n*sizeof(char) ; i++){

			if(*(token+i)==NULL){
				break;
			}
			printf("%s \n",*(token+i));
			
		}

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



