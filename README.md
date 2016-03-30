# tarea_1_so

Tarea 1 de Sistemas Operativos

La shell proporciona un prompt, lo que identifica el modo de espera de comandos de la shell. Luego, lee un comando desde teclado y parsea la entrada para identificar el comando y sus argumentos (soporta al menos 3 argumentos). Finalmente ejecuta el comando ingresado en un proceso
concurrente, para lo cual usa el llamado a sistema fork() y algunas de las variantes de exec(). Los comandos soportados son ejecutados en foreground, es decir, la shell ejecuta y espera por el término de su ejecución antes de imprimir el promtp para esperar por el siguiente comando.
