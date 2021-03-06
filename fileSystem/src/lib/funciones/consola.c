#include "../funcionesFS.h"

void consolaFS(){
	puts("Bienvenido a la consola. Ingrese un comando:");
	while(1){
		char *linea = readline(">");
		//esto se hace por que si el tipo apreta enter
		//rompe todo
		if(!strcmp(linea, "")){
			free(linea);
			continue;
		}
		add_history(linea);
		procesarInput(linea);
		free(linea);
	}
}

void procesarInput(char* linea) {
	int cantidad;
	char **palabras = string_split(linea, " ");
	cantidad = cantidadParametros(palabras);
	if (!strcmp(*palabras, "format")) {
		consolaFormat(palabras, cantidad);
	} else if (!strcmp(*palabras, "rm")) {
		consolaRemove(palabras,cantidad);
	} else if (!strcmp(*palabras, "rename")) {
		consolaRename(palabras,cantidad);
	} else if (!strcmp(*palabras, "mv")) {
		consolaMove(palabras,cantidad);
	} else if (!strcmp(*palabras, "cat")) {
		consolaCat(palabras,cantidad);
	} else if (!strcmp(*palabras, "mkdir")) {
		consolaMkdir(palabras,cantidad);
	} else if (!strcmp(*palabras, "cpfrom")) {
		consolaCpfrom(palabras,cantidad);
	} else if (!strcmp(*palabras, "cpto")) {
		consolaCpto(palabras,cantidad);
	} else if (!strcmp(*palabras, "cpblock")) {
		consolaCpblock(palabras, cantidad);
	} else if (!strcmp(*palabras, "md5")) {
		consolaMd5(palabras,cantidad);
	} else if (!strcmp(*palabras, "ls")) {
		consolaLs(palabras,cantidad);
	} else if (!strcmp(*palabras, "info")) {
		consolaInfo(palabras,cantidad);
	} else if(!strcmp(*palabras, "exit")){
		printf("Finalizando consola\n");
		liberarPunteroDePunterosAChar(palabras);
		free(palabras);
		free(linea);
		pthread_kill(pthread_self(),SIGKILL);
	} else {
		puts("El comando no existe.");
	}
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);

}

void consolaCpblock(char ** palabras, int cantidad){
	if(cantidad == 3){
		procesarCpblock(palabras);
	}
	else{
		puts("Error en la cantidad de parametros");
	}
}

void procesarCpblock(char ** palabras){
	int bloqueDN;
	if(verificarRutaArchivo(palabras[1])){
		char * rutaLocalArchivo = obtenerRutaLocalDeArchivo(palabras[1]);
		Tnodo *nodo = buscarNodoPorNombre(listaDeNodos, palabras[3]);
		if(nodo != NULL){
			if((bloqueDN = obtenerBloqueDisponible(nodo)) != -1){
				Tarchivo* tablaArchivo = malloc(sizeof(Tarchivo));
				int nroBloque = atoi(palabras[2]);
				Tbuffer* bloque;
				TbloqueAEnviar* bloqueAEnviar;
				levantarTablaArchivo(tablaArchivo, rutaLocalArchivo);

				if (nroBloque>=cantidadDeBloquesDeUnArchivo(tablaArchivo->tamanioTotal)) {
					puts("Numero de bloque incorrecto, intentelo de nuevo.");
					liberarTablaDeArchivo(tablaArchivo);
					free(rutaLocalArchivo);
					return;
				}

				if(existeBloqueEnNodo(nroBloque, nodo, tablaArchivo)){
					puts("Ya existe una copia del bloque en el nodo");
					liberarTablaDeArchivo(tablaArchivo);
					free(rutaLocalArchivo);
					return;
				}

				if (nodosDisponiblesParaBloqueDeArchivo(tablaArchivo, nroBloque) == 0) {
					puts("No se encontraron los nodos con las copias del bloque.");
					liberarTablaDeArchivo(tablaArchivo);
					free(rutaLocalArchivo);
					return;
				}
				pthread_mutex_init(&bloqueMutex, NULL);
				if (pedirBloque(tablaArchivo, nroBloque) == -1) {
					puts("Error al solicitar bloque.");
					liberarTablaDeArchivo(tablaArchivo);
					free(rutaLocalArchivo);
					return;
				}
				liberarTablaDeArchivo(tablaArchivo);
				pthread_mutex_lock(&bloqueMutex);
				pthread_mutex_lock(&bloqueMutex);
				bloque = malloc(sizeof(Tbuffer));
				if (copiarBloque(bloqueACopiar, bloque) == -1) {
					puts("Error al copiar bloque recibido.");
				}
				liberarEstructuraBuffer(bloqueACopiar);
				bloqueAEnviar = malloc(sizeof(TbloqueAEnviar));
				bloqueAEnviar->contenido = bloque->buffer;
				bloqueAEnviar->tamanio = bloque->tamanio;
				bloqueAEnviar->numeroDeBloque = bloqueDN;
				if (enviarBloqueA(bloqueAEnviar, palabras[3]) == -1) {
					puts("Error no se pudo enviar el bloque.");
					free(rutaLocalArchivo);
					liberarEstructuraBuffer(bloque);
					return;
				}
				agregarCopiaAtablaArchivo(rutaLocalArchivo,palabras[3],bloqueDN,nroBloque);
				ocuparBloque(nodo, bloqueDN);
				free(rutaLocalArchivo);
				liberarEstructuraBuffer(bloque);
				puts("El bloque se copio exitosamente.");
			}
			else{
				puts("El nodo destino esta lleno.");
			}
		}
		else {
			puts("El nodo destino no existe o no esta conectado.");
		}
	}
	else{
		puts("Ruta de archivo incorrecta.");
	}
}

void consolaFormat(char**palabras, int cantidad){
	if(cantidad == 0){
		formatearFS();
	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}

void consolaCpto(char** palabras, int cantidad) {
	if (cantidad == 2) {
		if (verificarRutaArchivo(palabras[1])) {
			DIR* dir = opendir(palabras[2]);
			if(dir){
				if (copiarArchivo(palabras) != -1) {
					puts("Se copio el archivo correctamente.");
				} else {
					puts("No se pudo copiar el archivo.");
				}
				closedir(dir);
			}
			else{
				puts("El directorio especificado, no existe. Por favor, creelo e intente otra vez.");
			}
		}
		else{
			puts("La ruta especificado es incorrecta.");
		}
	} else {
		puts("Error en la cantidad de parametros");
	}
}

void consolaRename(char** palabras, int cantidad) {
	if (cantidad == 2) {
		if (esRutaYamafs(palabras[1])){
			if(!esRutaYamafs(palabras[2])){
				int esRutaARoot = string_ends_with(palabras[1], "yamafs:/");
				if (!esRutaARoot) {
					if(esRutaDeUnArchivo(palabras[1])){
						if(verificarRutaArchivo(palabras[1])){
							renombrarArchivo(palabras[1],palabras[2]);
						}
					}
					else {
						if (existeDirectorio(palabras[1])) {
							renombrarDirectorio(palabras[1], palabras[2]);
						}
						else {
							puts("El directorio no existe");
						}
					}
				}
				else {
					puts("No se puede renombrar el root.");
				}
			}
			else{
				puts("El segundo parametro tiene que ser un nombre, no una ruta yamafs");
			}
		}
		else {
			puts("Falta la referencia a yamafs:/.");
		}
	} else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaCat(char ** palabras, int cantidad) {
	if (cantidad == 1) {
		if(esRutaYamafs(palabras[1])){
			if(verificarRutaArchivo(palabras[1])){
				char * rutaLocal = obtenerRutaLocalDeArchivo(palabras[1]);
				leerArchivoComoTextoPlano(rutaLocal);
				free(rutaLocal);
			}
			else{
				puts("La ruta especificada no corresponde a ningun archivo de yamafs.");
			}
		} else{
			puts("Falta la referencia a yamafs:/.");
		}
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaMkdir(char**palabras, int cantidad){
	if(cantidad == 1){
		if(esRutaYamafs(palabras[1])){
			if(existeDirectorio(palabras[1])){
				puts("El directorio ya existe.");
			}
			else{
				puts("No existe el directorio.");
				if(crearDirectorio(palabras[1])>=0){
					persistirTablaDeDirectorios();
				}
			}
		}
		else{
			puts("Falta la referencia a yamafs:/.");
		}
	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}

void consolaCpfrom(char** palabras, int cantidad){
	if(cantidad == 2){
		if(esRutaYamafs(palabras[2])){
			if(existeDirectorio(palabras[2])){
				if(archivoRepetidoEnDirectorio(palabras[1],palabras[2])){
					if(almacenarArchivo(palabras)){
						puts("Se ha almacenado el archivo en el yamafs con exito.");
					}
				}
				//que significa este else return? no deberia tirar un mensaje ?? todo
				else return;
			}
			else {
				puts("No existe el directorio.");
			}
		}
		else{
			puts("Falta la referencia a yamafs en el segundo parametro.");
		}
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaMd5(char** palabras, int cantidad){
	if (cantidad == 1){
		if(verificarRutaArchivo(palabras[1])){
			getMD5(palabras[1]);
		}
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaLs(char**palabras, int cantidad){
	if(cantidad == 1){
		if(esRutaYamafs(palabras[1])){
			if(existeDirectorio(palabras[1])){
				listarArchivos(palabras[1]);
			}
			else{
				puts("No existe el directorio.");
			}
		}
		else{
			puts("Falta la referencia a yamafs.");
		}
	}
	else if(cantidad == 2){
		if(!strcmp(palabras[1], "-d")){
			if(esRutaYamafs(palabras[2])){
				if(!esRutaDeUnArchivo(palabras[2])){
					mostrarArbolDeDirectorios(palabras[2]);
				}
				else{
					puts("La ruta ingresada, no es una ruta a un directorio.");
				}
			}
			else{
				puts("Falta la referencia a yamafs.");
			}
		}
		else{
			puts("Error, use el prefijo -d para mostrar el árbol de directorios de una ruta.");
		}
	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}

void consolaInfo(char**palabras, int cantidad) {
	if (cantidad == 1) {
		if(!strcmp(palabras[1], "--all")){
			mostrarDistribucionDeBloquesEnNodos();
		}
		else if (verificarRutaArchivo(palabras[1])) {
			Tarchivo* tablaArchivo = malloc(sizeof(Tarchivo));
			char * rutaLocal = obtenerRutaLocalDeArchivo(palabras[1]);
			levantarTablaArchivo(tablaArchivo, rutaLocal);
			mostrarTablaArchivo(tablaArchivo);
			liberarTablaDeArchivo(tablaArchivo);
			free(rutaLocal);
		}
		else{
			puts("Error en los parametros. Utilice una ruta para mostrar los contenido de ese archivo o --all para mostrar todo.");
		}
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaRemove (char** palabras, int cantidad){
	if (cantidad == 1){
		if(verificarRutaArchivo(palabras[1])){
			if(removerArchivo(palabras[1])){
				puts("El archivo se elimino correctamente.");
			}
			else{
				puts("No se pudo completar la operacion, el nodo con la copia no esta en ningun lado.");
			}
		}
	}
	else if (cantidad ==2){
		if (!strcmp(palabras[1], "-d")){
			if(esRutaYamafs(palabras[2])){
				if(existeDirectorio(palabras[2])){
					if(esDirectorioRaiz(palabras[2])){
						puts("No se puede remover el directorio raíz.");
					}
					else if(esDirectorioVacio(palabras[2])){
						removerDirectorio(palabras[2]);
						puts("El directorio se elimino correctamente.");
					}
					 else{
						puts("El directorio no esta vacío, por lo tanto, no se puede remover.");
					}
				}
				else{
					puts("No existe el directorio a remover.");
				}
			}
		}
		else{
			puts("Error, utilice el prefijo -d para borrar un directorio.");
		}
	}
	else if(cantidad == 4){
		if (!strcmp(palabras[1], "-b")){
			if(verificarRutaArchivo(palabras[2])){
				char* rutaLocalArchivo = obtenerRutaLocalDeArchivo(palabras[2]);
				Tarchivo* tablaArchivo = malloc(sizeof(Tarchivo));
				levantarTablaArchivo(tablaArchivo, rutaLocalArchivo);
				free(rutaLocalArchivo);
				int nroBloque = atoi(palabras[3]);
				if(esBloqueValido(tablaArchivo, nroBloque)){
					int nroCopia = atoi(palabras[4]);
					if (esCopiaValida(tablaArchivo->bloques[nroBloque], nroCopia)) {
						char * rutaLocal = obtenerRutaLocalDeArchivo(palabras[2]);
						eliminarBloqueDeUnArchivo(rutaLocal, nroBloque, nroCopia);
						free(rutaLocal);
						liberarTablaDeArchivo(tablaArchivo);
					}
					else{
						puts("Numero de Copia incorrecta.");
						liberarTablaDeArchivo(tablaArchivo);
					}
				}
				else{
					puts("Numero de bloque incorrecto.");
					liberarTablaDeArchivo(tablaArchivo);
				}
			}
			else{
				puts("Ruta de archivo incorrecta.");
			}
		}
		else{
			puts("Error, utilice el prefijo -b para borrar un bloque.");
		}
	}

	else{
		puts("Error en la cantidad de parámetros.");
	}
}

int getMD5(char* rutaYamafs){
	char* rutaLocal = obtenerRutaLocalDeArchivo(rutaYamafs);
	char * nombreArchivoConExtension = obtenerNombreDeArchivoDeUnaRuta(rutaLocal);
	char * rutaTmp = string_new();
	Tarchivo * archivo = malloc(sizeof(Tarchivo));
	levantarTablaArchivo(archivo,rutaLocal);

	mkdir("/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/metadata/tmp/", 0777);
	string_append(&rutaTmp, "/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/metadata/tmp/");
	string_append(&rutaTmp, nombreArchivoConExtension);
	if(levantarArchivo(archivo,rutaTmp) == -1){
		puts("Error al obtener el MD5");
		log_error(logError, "Error al levantar el archivo, dentro de la operacion getMD5");
		return -1;
	}
	char* comando = string_duplicate("md5sum ");
	string_append(&comando, rutaTmp);
	system(comando);

	remove(rutaTmp);
	free(comando);
	free(rutaTmp);
	free(rutaLocal);
	free(nombreArchivoConExtension);
	liberarTablaDeArchivo(archivo);
	return 0;
}

void consolaMove(char ** palabras, int cantidad) {
	if (cantidad == 2) {
		char** carpetas1 = string_split(palabras[1], "/");
		char** carpetas2 = string_split(palabras[2], "/");
		if (!esRutaYamafs(carpetas1[0])	|| !esRutaYamafs(carpetas2[0])) {
			puts("Falta la referencia a yamafs:/ en alguna de las rutas.");
		} else {
			if(!esRutaDeUnArchivo(palabras[2])){
				if(existeDirectorio(palabras[2])){
					if (esRutaDeUnArchivo(palabras[1])) {
						if (verificarRutaArchivo(palabras[1])) {
							moverArchivo(palabras[1], palabras[2]);
						}
					}
					else {
						if(existeDirectorio(palabras[1])){
							moverDirectorio(palabras[1],palabras[2]);
						}
						else{
							puts("No se puede mover un directorio que no existe.");
						}
					}
				}
				else {
					puts("No existe el directorio del segundo parametro");
				}
			} else{
				puts("El segundo parametro tiene que ser una ruta de directorio, no de archivo");
			}
		}

		liberarPunteroDePunterosAChar(carpetas1);
		free(carpetas1);
		liberarPunteroDePunterosAChar(carpetas2);
		free(carpetas2);
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

