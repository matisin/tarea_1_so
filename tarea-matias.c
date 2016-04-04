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
	pid_t pid;

	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	char **tokens_2 = (char **) malloc(buffer_n*sizeof(char));

	int p[2],hay_pipe,i,temp,mishell_log;

	system("clear");

	//Se crea la carpeta log.
	pid = fork();
	if(pid == 0){		
		execl("/bin/mkdir","mkdir","Log",NULL);
	}else{
		//se espera al hijo para continuar
		int status;
		(void)waitpid(pid, &status, 0);
	}

	while(1) {		 	
		
		printPrompt();//imprime el prompt

		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}			

		mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
		write(mishell_log,"@",1);
		write(mishell_log,input,strlen(input)); //se guarda el comando leído en el log
		close(mishell_log);

		hay_pipe = parser(input,tokens,tokens_2); //se guardan los tokens y se retorna si hay pipe o no	

		if(hay_pipe){	

			pipe(p);//creamos el pipe
			pid = fork();
			if(pid == 0){	
					
				dup2(p[1],1);//se redirecciona la salida del hijo que se ejecuta primero al pipe.	
				close(p[0]);//cerramos la entrada del pipe.		
				checkcommand(tokens);//ejecutamos los comandos.
				exit(EXIT_FAILURE);

			}else{

				int status;
				(void)waitpid(pid, &status, 0);//se espera al hijo

				pid = fork();
				if(pid == 0){

					dup2(p[0],0);//se redirecciona la entra del hijo que se ejecuta segundo al pipe.			
					close(p[1]);//cerramos la salida del pipe

					//se abre el archivo que guarda el log temporalmente
					temp = open(".logtemp",O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);				
					dup2(temp,1);//se envia la salida al archivo
					close(temp);									
					checkcommand(tokens_2); 
					exit(EXIT_FAILURE);

				}else{

					close(p[0]);
					close(p[1]);
					int status;
					(void)waitpid(pid, &status, 0);
					mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);		
					temp = open(".logtemp",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					char c;
					while (read(temp, &c, sizeof(char)) != 0) {
						//se copia el contenido de el log temporal al log y a la salida estandar.
    					printf("%c", c);
    					write(mishell_log,&c,sizeof(char));
    				
  					}
  					//se cierran los archivos y se borra el log temporal.
  					close(temp);
  					close(mishell_log);
  					remove(".logtemp");
					
				}								
			}
		}			

		else{			

			pid = fork();

			if (pid == -1) { //si falla el fork
				perror("fork failed");
				exit(EXIT_FAILURE);
			}
			
			else if (pid == 0) { //hijo

				//se abre el archivo que guarda el log temporalmente
				temp = open(".logtemp",O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);				
				dup2(temp,1);//se envia la salida al archivo
				close(temp);
				//se ejecuta el comando
				checkcommand(tokens);				
				exit(EXIT_FAILURE);					
			}
			
			else {//padre

				int status;
				(void)waitpid(pid, &status, 0); //Esto hace que el padre espere que termine el hijo.
				mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);		
				temp = open(".logtemp",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				char c;
				while (read(temp, &c, sizeof(char)) != 0) {
					//se copia el contenido de el log temporal al log y a la salida estandar.
    				printf("%c", c);
    				write(mishell_log,&c,sizeof(char));
    				
  				}
  				//se cierran los archivos y se borra el log temporal.
  				close(temp);
  				close(mishell_log);
  				remove(".logtemp"); 			
				
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

	
}