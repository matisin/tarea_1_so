#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define   buffer_n 1000

// parsea un string de entrada
//
// return char **
int parser(char *string,char **tokens){	

	int hay_pipe = 0;//se asume que no hay pipe
	char *delim = " \t\n";
	char *token =  strtok(string,delim);	
	int i = 0;

	//recorremos los tokens
	while(token){
		*(tokens+i) =  token;//guardamos los tokens
		if(!hay_pipe){
			if(*token=='|'){
				hay_pipe=1;//si encuentra pipe se modifica a 1.
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

int main (int argc, char *argv[]) {
	
	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	int hay_pipe; 
	int i;

	system("clear");

	while(1) {		

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada.

		if(*input == '\n') { //se compara el primer caracter primero.
			continue;
		}			

		hay_pipe=parser(input,tokens); //se guardan los tokens y veificamos si hay pipe.
		
		//Para imprimir tokens
		/*for(i = 0 ;i < buffer_n*sizeof(char) ; i++){

			if(*(tokens+i)==NULL){
				break;
			}
			printf("%s \n",*(tokens+i));
		}*/	

		if(hay_pipe){

			printf("Hay pipe!\n");

		
		}else{

			pid_t pid = fork();
			//REVISA SI FORK FALLA

			if (pid == -1) {

				perror("fork failed");
				exit(EXIT_FAILURE);
			}else if (pid == 0) { // ESTE ES EL PROCESO HIJO

				printf("Hello from the child process!\n");
				exit(EXIT_SUCCESS);
			}else {	// ESTE ES EL PROCESO PADRE	
				int status;
				(void)waitpid(pid, &status, 0); //Esto hace que el padre espere que termine el hijo.
				//lo que haga el proceso padre tiene que ir despues de esta llamada. 
			}
		}
		

		//reseteamos el input y los tokens.
		memset(input,0,buffer_n*sizeof(char));
		memset(tokens,0,buffer_n*sizeof(char));

	}

	free(input);
	free(tokens);

	printf("Donoso culiao\n");
}