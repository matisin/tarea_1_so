#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define   buffer_n 1000

// parsea un string de entrada
//
// return char **
int parser(char *string,char **tokens){	
	int hay_pipe = 0;
	char *delim = " \t\n";
	char *token =  strtok(string,delim);	
	int i = 0;
	while(token){			
		*(tokens+i) =  token;
		if(!hay_pipe){
			if(*token=='|'){
				hay_pipe=1;
			}
		}
		token = strtok(NULL, delim);
		i++;
	}
	return hay_pipe;
}

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}
void busca_pipe(int *hay_pipe,char **tokens){

}
int main (int argc, char *argv[]) {
	
	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	int hay_pipe; 
	int i;

	system("clear");

	while(1) {		

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}			

		hay_pipe=parser(input,tokens); //se guardan los tokens 
		
		//Para imprimir tokens
		for(i = 0 ;i < buffer_n*sizeof(char) ; i++){

			if(*(tokens+i)==NULL){
				break;
			}
			printf("%s \n",*(tokens+i));
		}		
		if(hay_pipe){
			printf("Hay pipe!\n");
		}else{
			printf("No hay pipe!\n");
		}
		//reseteamos el input y los tokens.
		memset(input,0,buffer_n*sizeof(char));
		memset(tokens,0,buffer_n*sizeof(char));

	}

	free(input);
	free(tokens);

	printf("Donoso culiao\n");
}



