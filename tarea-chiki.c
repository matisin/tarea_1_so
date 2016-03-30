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
			pid_t pid = fork();

			//REVISA SI FORK FALLA
			if (pid == -1) {
				perror("fork failed");
				exit(EXIT_FAILURE);
			}
			// ESTE ES EL PROCESO HIJO
			else if (pid == 0) {
				printf("Hello from the child process!\n");
				_exit(EXIT_SUCCESS);
			}
			// ESTE ES EL PROCESO PADRE
			else {				
				int status;
				(void)waitpid(pid, &status, 0); //Esto hace que el padre espere que termine el hijo.
				//lo que haga el proceso padre tiene que ir despues de esta llamada. 
			}
		}

	}

	free(input);
	free(token);

	printf("Donoso culiao\n");
}



