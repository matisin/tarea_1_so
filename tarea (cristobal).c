#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define   buffer_n 1000
#define   SIZE 512

// parsea un string de entrada
//
// return char **
int parser(char *string,char **tokens_1,char **tokens_2){	

	int hay_pipe = 0;
	char *delim = " \t\n";
	char *token =  strtok(string,delim);	
	int i = 0;

	while(token){			
		
		if(!hay_pipe){
			if(*token=='|'){
				hay_pipe=1;
			}else {
				*(tokens_1+i) =  token;
			}
		}else{
			*(tokens_2+i) =  token;
		}
		token = strtok(NULL, delim);
		i++;
	}

	return hay_pipe;
}
/*
FUNCION QUE DEVUELVE LA EJECUCIÓN DE LOS COMANDOS, SOLO AQUELLOS
QUE SE ENCUENTREN EN /BIN, OSEA EJECUTABLES, POR EJ: ECHO, LS...
*/
void checkcommand(char **tokens){
	char *args[] = {tokens[0], tokens[1],tokens[2],(char *) 0 };
	execvp(tokens[0],args);	
}

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}

int main (int argc, char *argv[]) {
	pid_t pid;
	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	int p[2],hay_pipe,i,readbytes;
	pipe(p);
	char buffer[SIZE];

	system("clear");

	while(1) {		

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}			

		hay_pipe = parser(input,tokens); //se guardan los tokens 
		

		if(hay_pipe){

			if ( (pid=fork()) == 0 )
  			{ // hijo
    			close( p[1] ); /* cerramos el lado de escritura del pipe */
 				char leidos;
 				while((leidos = read(p[0], &tokens[i], sizeof tokens[i]))>0)
 					printf("%d Recibido %s\n", getpid(), tokens[i]);
  			}
  		else
  			{ // padre
    			close( p[0] ); /* cerramos el lado de lectura del pipe */
 
				for (i = 0; i < sizeof tokens && tokens[i] != NULL; i++)
				{
					write(p[1], &tokens[i], sizeof tokens[i]);
					printf("%d Escribiendo %s\n", getpid(), tokens[i]);
					sleep(1);
				}
  			}

		}else{

				pid_t pid = fork();
				//REVISA SI FORK FALLA

				if (pid == -1) {

					perror("fork failed");
					exit(EXIT_FAILURE);
				}

				// ESTE ES EL PROCESO HIJO
				else if (pid == 0) {
					checkcommand(tokens);

					printf("Hello from the child process!\n");
					exit(EXIT_SUCCESS);
				}

				// ESTE ES EL PROCESO PADRE
				else {			

					int status;
					(void)waitpid(pid, &status, 0); //Esto hace que el padre espere que termine el hijo.
					//lo que haga el proceso padre tiene que ir despues de esta llamada.
					printf("Hola soy el papá\n"); 
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