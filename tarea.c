#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <time.h>

#define   buffer_n 1000

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
			if(*token=='|')//si se encuentra el pipe
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
//ejecuta comando sin pipe
//
// return void
void command_normal(char **tokens){

	pid_t pid;
	clock_t begin, end;
	double time_spent;
	int p[2],temp,mishell_log;

	begin = clock();//para medir el tiempo
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
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		char str[17];
  		sprintf(str,"tiempo: %f\n", time_spent);
		mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);		
		temp = open(".logtemp",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		write(mishell_log,str,strlen(str));
		char c;
		while (read(temp, &c, sizeof(char)) != 0) {
			//se copia el contenido de el log temporal al log y a la salida estandar.
    		printf("%c", c);
    		write(mishell_log,&c,sizeof(char));
    				
  		}
  				
  		//se cierran los archivos y se borra el log temporal.
  		close(temp);
  		write(mishell_log,"~",1);
  		close(mishell_log);
  		remove(".logtemp"); 			
				
	}
}
//ejecuta comando con pipe
//
// return void
void command_pipe(char **tokens, char **tokens_2){

	pid_t pid;
	clock_t begin, end;
	double time_spent;
	int p[2],temp,mishell_log;

	begin = clock();
	pipe(p);//creamos el pipe
	pid = fork();

	if (pid == -1) { //si falla el fork

		perror("fork failed");
		exit(EXIT_FAILURE);

	}else if(pid == 0){	
					
		dup2(p[1],1);//se redirecciona la salida del hijo que se ejecuta primero al pipe.	
		close(p[0]);//cerramos la entrada del pipe.		
		checkcommand(tokens);//ejecutamos los comandos.
		exit(EXIT_FAILURE);

	}else{

		int status;
		(void)waitpid(pid, &status, 0);//se espera al hijo

		pid = fork();
		if(pid == 0){

			dup2(p[0],0);//se redirecciona la entrada del hijo que se ejecuta segundo al pipe.			
			close(p[1]);//cerramos la salida del pipe

			//se abre el archivo que guarda el log temporalmente
			temp = open(".logtemp",O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);				
			dup2(temp,1);//se envia la salida al archivo
			close(temp);									
			checkcommand(tokens_2); 
			exit(EXIT_FAILURE);

		}else{//padre que espera los dos procesos

			close(p[0]);
			close(p[1]);
			int status;
			(void)waitpid(pid, &status, 0);
			//para medir el tiempo.
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			char str[17];
  			sprintf(str,"tiempo: %f\n", time_spent);
			mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);		
			temp = open(".logtemp",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			write(mishell_log,str,strlen(str));
			char c;
			while (read(temp, &c, sizeof(char)) != 0) {
				//se copia el contenido de el log temporal al log y a la salida estandar.
    			printf("%c", c);
    			write(mishell_log,&c,sizeof(char));
    				
  			}  			
  			//se cierran los archivos y se borra el log temporal.
  			close(temp);
  		
			write(mishell_log,"~",1);
				
  			close(mishell_log);

  			remove(".logtemp");
					
		}							
	}
}
// se buscan las instancias de un comando en el log (salida + tiempo)
//
// return void
void search_command(char **tokens){

	int len = strlen(tokens[1]);//largo del comando
	int mishell_log,i;

	mishell_log= open("Log/mishell.log",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);//se abre el log
	char c;
	while (read(mishell_log, &c, sizeof(char)) != 0) {//recoremos hasta el fin de string
		
    	if(c == '@'){//el @ marca un comando en el log
    		read(mishell_log, &c, sizeof(char));
    		if(c == tokens[1][0]){
    			for(i = 0 ; i < len - 1; i++){
    				read(mishell_log, &c, sizeof(char));
    				if(c != tokens[1][i+1]){
    					break;//si el comando es distinto, seguimos buscando
    				}
    				if(i == len - 2){//si el comando es el mismo
    					read(mishell_log, &c, sizeof(char));
    					while(c != '\n'){//avanzamos por la linea completa del comando
    						read(mishell_log, &c, sizeof(char));
    					}
    					read(mishell_log, &c, sizeof(char));
    					while(c != '~'){//copiamos todo bajo la linea del comando hasta encontrarnos con otro comando y empieza otra vez.
    						printf("%c",c);
    						read(mishell_log, &c, sizeof(char));
    					}
    				}
    			}
    		}
    	}
    				
  	} 
  	close(mishell_log);//se cierra el archivo.

}
//despliega la lista de comandos utilizados en la sesión.
void listCommand(int contador){
	char *line = NULL;
	FILE *fp;
	size_t len = 0;
	ssize_t read;

	int mishell_log;

	fp = fopen("Log/.templog","r");//abrimos el archivo que contiene los comandos de la sesión
	if(fp == NULL){
		perror("Error opening log file");
		exit(EXIT_FAILURE);
	}
	//abrimos el log para escribir lo que hicimos
	mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	

	char number[4];
	int count = 1;
	while( (read = getline(&line, &len, fp)) != -1 ){
		if(count == contador){//si llegamos al final de la lista
			break;
		}
		sprintf(number,"%d.- ", count);
		write(mishell_log,number,strlen(number)); //se guarda en numero de lista en el log
		write(mishell_log,line,strlen(line)); //se guarda el comando leído en el log
		printf("%d.- ", count);
		printf("%s",line);
		count++;
	}
	close(mishell_log);
	fclose(fp);
}
//entrega una lista de comandos y ejecuta el seleccionado
//
// return void
void chooseCommand(char **tokens, char **tokens_2, int *contador){
	int num,hay_pipe,mishell_log;
	listCommand(*contador);//despliega la lista
	mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
	write(mishell_log,"Seleccione número del comando que desea volver a ejecutar o presione 0 para salir.\n",84);	
	printf("Seleccione número del comando que desea volver a ejecutar o presione 0 para salir.\n" );
	//////
	fflush(stdin);
	scanf("%d", &num);
	fflush(stdin);
	/////
	char str[4];
	sprintf(str,"%d\n",num);
	write(mishell_log,str,strlen(str));//se guarda el numero seleccionado
	close(mishell_log);

	if(num != 0){//si no se seleccionó el 0
		if(num >= *contador){//si el numero es mayor que el numero de comandos usados, el comando no existiria.
			printf("Error, no existe comando\n");
			mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
			write(mishell_log,"Error, no existe comando\n",25);
			close(mishell_log);
		}
		else{
			char *line = NULL;
			FILE *fp;
			size_t len = 0;
			ssize_t read;	

			fp = fopen("Log/.templog","r");//abrimos el archivo que tiene los comandos de la sesión
			if(fp == NULL){
				perror("Error opening log file");
				exit(EXIT_FAILURE);
			}
			int count = 1;
			while( (read = getline(&line, &len, fp)) != -1 ){
				if(num == count){//si avanzamos hasta la linea seleccionada
					printf("%s",line);//se imprime el comando
					mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
					write(mishell_log,"@",1);
					write(mishell_log,line,strlen(line)); //se guarda el comando leído en el log
					close(mishell_log);

					hay_pipe = parser(line,tokens,tokens_2);//obtienen los tokens y la informacion sobre pipe

					if(hay_pipe){	

						command_pipe(tokens,tokens_2);//ejecutamos los comandos con pipe		
					}				

					else{
						if(strcmp(tokens[0], "DELETE_LOG") == 0 ){//si el comando es delete log se borra el log
							remove("Log/mishell.log");
							printf("Se ha borró el log \n");						
							
						}					

						else if(strcmp(tokens[0], "CHOOSE_COMMAND") == 0){//no se puede ejecutar choose_command de nuevo
							printf("No se puede hacer CHOOSE_COMMAND desde CHOOSE_COMMAND\n");					
						
						}			

						else if(strcmp(tokens[0], "LIST_COMMAND") == 0){
							printf("No se puede hacer LIST_COMMAND desde CHOOSE_COMMAND\n");
							
						}else if(strcmp(tokens[0],"SEARCH_COMMAND") == 0){

							search_command(tokens);
							
						}
						else {
							command_normal(tokens);	//ejecutamos los comandos sin pipe

						}
												
					}	
					*contador++;//aumentamos la cantidad de comandos ya que agregamos el comando seleccionado
					
				}
				count++;
			}
			fclose(fp);

		}
		
	}	

}

int main (int argc, char *argv[]) {
	pid_t pid;

	char *input = (char *) malloc(buffer_n*sizeof(char));
	char **tokens = (char **) malloc(buffer_n*sizeof(char));
	char **tokens_2 = (char **) malloc(buffer_n*sizeof(char));
	
	int hay_pipe,i,mishell_log,templog;
	int contador = 0;

	system("clear");
	remove("Log/.templog");

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

		//reseteamos el input y los tokens.
		memset(input,0,buffer_n*sizeof(char));
		memset(tokens,0,buffer_n*sizeof(char));
		memset(tokens_2,0,buffer_n*sizeof(char));	
		
		printPrompt();//imprime el prompt

		
		fgets(input,buffer_n*sizeof(char),stdin);//leemos la entrada

		if(*input == '\n') { //se compara el primer caracter primero
			continue;
		}			

		mishell_log= open("Log/mishell.log",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
		write(mishell_log,"@",1);
		write(mishell_log,input,strlen(input)); //se guarda el comando leído en el log
		close(mishell_log);

		templog= open("Log/.templog",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	
		write(templog,input,strlen(input)); //se van guardando en un archivo aparte los commandos de la sesión.
		close(templog);

		contador++;//contador de comandos de la sesión.

		hay_pipe = parser(input,tokens,tokens_2); //se guardan los tokens y se retorna si hay pipe o no	

		if(strcmp(tokens[0], "DELETE_LOG") == 0 ){
			remove("Log/mishell.log");
			printf("Se ha borrado el log \n");
			continue;
		}

		if(strcmp(tokens[0], "QUIT") == 0 ){
			break;
		}

		if(strcmp(tokens[0], "CHOOSE_COMMAND") == 0){
			chooseCommand(tokens,tokens_2,&contador);
			continue;
		}

		if(strcmp(tokens[0], "LIST_COMMAND") == 0){
			listCommand(contador);
			continue;
		}

		if(strcmp(tokens[0],"SEARCH_COMMAND") == 0){
			search_command(tokens);
			continue;
		}

		if(hay_pipe){

			command_pipe(tokens,tokens_2);	
			continue;
			
		}			

		else{

			command_normal(tokens);	
			continue;		
		}		
		

	}	
	free(input);
	free(tokens);
	free(tokens_2);

	
}