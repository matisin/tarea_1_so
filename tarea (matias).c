#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h> 
#include <fcntl.h>


#define   buffer_n 1000
#define   SIZE 512

// parsea un string de entrada
//
// return char **
int parser(char *string,char **tokens,char **tokens_2){	

	int hay_pipe = 0;
	char *delim = " \t\n";
	char *token =  strtok(string,delim);	
	int i = 0, j=0;

	while(token){			
		
		if(!hay_pipe)
		{
			if(*token=='|')
			{
				hay_pipe=1;
				
			}else{
				*(tokens+i) =  token;
			}
		}else{
			*(tokens_2+j) =  token;
			j++;
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
	char *args[] = {tokens[0], tokens[1],tokens[2],tokens[3],(char *) 0 };
	execvp(tokens[0],args);
	printf("Command execute error in %s: %s\n", tokens[0],strerror(errno) );	
	
}

// imprime el prompt
//
// return void
void printPrompt(){

	printf("minishell$ ");
}

int main (int argc, char *argv[]) {
	pid_t pid,pid2;
	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	char **tokens_2 = (char **) malloc(buffer_n*sizeof(char));
	int p[2],hay_pipe,i,readbytes,out;
	
	char buffer[SIZE];
	int status = 0;

//	out = open("log",O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
 //	dup2(out,0);
 	//close(out);
	system("clear");
	pid = fork();
	if(pid == 0){
		char *logs[] = {
        "mkdir",       
        "Log",
        NULL
    	};
		execvp(logs[0],logs);
	}else{
		int status;
		(void)waitpid(pid, &status, 0);
	}
	out = open("Log/mishel.log",O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
 	
 


	while(1) {		

		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}			

		hay_pipe = parser(input,tokens,tokens_2); //se guardan los tokens 
		

		if(hay_pipe){	
			pipe(p);
			pid = fork();
			if(pid == 0){	
					
				dup2(p[1],1);
				close(p[1]);
				
				checkcommand(tokens);
				exit(EXIT_FAILURE);
			}else{
				int status;
				(void)waitpid(pid, &status, 0);

				pid = fork();
				if(pid == 0){
					dup2(p[0],0);
					close(p[0]);
									
					checkcommand(tokens_2); 
					exit(EXIT_FAILURE);
				}else{
					close(p[0]);
					close(p[1]);
					int status;
					(void)waitpid(pid, &status, 0);
					
				}								
			}
		}			

		else{

			pipe(p);

			pid = fork();
			//REVISA SI FORK FALLA

			if (pid == -1) {
				perror("fork failed");
				exit(EXIT_FAILURE);
			}

			// ESTE ES EL PROCESO HIJO
			else if (pid == 0) {
				dup2(out,1);
				close(out);
				checkcommand(tokens);
			
				exit(EXIT_FAILURE);
					
			}

			// ESTE ES EL PROCESO PADRE
			else {			

				int status;
				(void)waitpid(pid, &status, 0); //Esto hace que el padre espere que termine el hijo.
				//lo que haga el proceso padre tiene que ir despues de esta llamada.
				
			}
			
		}

		//reseteamos el input y los tokens.
		memset(input,0,buffer_n*sizeof(char));
		memset(tokens,0,buffer_n*sizeof(char));
		memset(tokens_2,0,buffer_n*sizeof(char));

	}

	free(input);
	free(tokens);
	free(tokens_2);

	printf("Donoso culiao\n");
}