#include "funcionesFS.h"

void almacenarBloquesEnEstructuraArchivo(Tarchivo * estructuraArchivoAAlmacenar, Tnodo * nodo1, Tnodo * nodo2, TbloqueAEnviar * bloque){
	estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaCero.nombreDeNodo = malloc(TAMANIO_NOMBRE_NODO);
	strcpy(estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaCero.nombreDeNodo, nodo1->nombre);
	printf("El nombre de nodo es %s\n", estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaCero.nombreDeNodo);

	estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaCero.numeroBloqueDeNodo = nodo1->primerBloqueLibreBitmap;
	ocuparProximoBloque(nodo1);
	mostrarBitmap(nodo1->bitmap);

	estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaUno.nombreDeNodo = malloc(TAMANIO_NOMBRE_NODO);
	strcpy(estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaUno.nombreDeNodo, nodo2->nombre);
	printf("El nombre de nodo es %s\n", estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaUno.nombreDeNodo);

	estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].copiaUno.numeroBloqueDeNodo = nodo2->primerBloqueLibreBitmap;
	ocuparProximoBloque(nodo2);
	mostrarBitmap(nodo2->bitmap);

	estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].bytes = bloque->tamanio;
	printf("El tamaño del bloque en bytes es: %llu", estructuraArchivoAAlmacenar->bloques[bloque->numeroDeBloque].bytes);
}

void ocuparProximoBloque(Tnodo * nodo){
	bitarray_set_bit(nodo->bitmap, nodo->primerBloqueLibreBitmap);
	nodo->primerBloqueLibreBitmap++;
	nodo->cantidadBloquesLibres--;
	ocuparBloqueEnTablaArchivos(nodo->nombre);
}

void enviarBloque(TbloqueAEnviar* bloque, Tarchivo * estructuraArchivoAAlmacenar){
	Theader * head = malloc(sizeof(Theader));
	Tbuffer * buffer1;
	Tbuffer * buffer2;
	int estado;
	head->tipo_de_proceso=FILESYSTEM;
	head->tipo_de_mensaje=ALMACENAR_BLOQUE;

	list_sort(listaDeNodos, ordenarSegunBloquesDisponibles);
	Tnodo* nodo1 = (Tnodo*)list_get(listaDeNodos, 0);
	Tnodo* nodo2 = (Tnodo*)list_get(listaDeNodos, 1);
	//Tnodo* nodo3 = (Tnodo*)list_get(listaDeNodos, 2);
	//Tnodo* nodo4 = (Tnodo*)list_get(listaDeNodos, 3);

	buffer1 = empaquetarBloque(head,bloque,nodo1);

	printf("Numero de bloque %d , Tamanio de bloque %llu\n", bloque->numeroDeBloque,bloque->tamanio);
	printf("Tamanio del buffer que se va a enviar %llu \n", buffer1->tamanio);
	 if ((estado = send(nodo1->fd, buffer1->buffer , buffer1->tamanio, 0)) == -1){
		 logAndExit("Fallo al enviar a Nodo el bloque a almacenar");
	 }

	buffer2 = empaquetarBloque(head,bloque,nodo2);
	printf("Se envio bloque a Nodo1 %d bytes\n", estado);
	 if ((estado = send(nodo2->fd, buffer2->buffer , buffer2->tamanio, 0)) == -1){
		 logAndExit("Fallo al enviar a Nodo el bloque a almacenar");
	 }
	/*double obtenerProporcionDeDisponibilidad(Tnodo* nodo){
		if(nodo->cantidadBloquesLibres == 0) return 1;
		double bloquesOcupados = nodo->cantidadBloquesTotal - nodo->cantidadBloquesLibres;
		return bloquesOcupados / nodo->cantidadBloquesTotal;
	}*/
	/*
	double p1 = obtenerProporcionDeDisponibilidad(nodo1);
	double p2 = obtenerProporcionDeDisponibilidad(nodo2);
	double p3 = obtenerProporcionDeDisponibilidad(nodo3);
	double p4 = obtenerProporcionDeDisponibilidad(nodo4);
	FILE * archivoDeSeguimiento = fopen("/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/envioBloques.txt","a");
	fseek(archivoDeSeguimiento,0,SEEK_END);
	fprintf(archivoDeSeguimiento, "%s %d \n","Bloque nro: ", bloque->numeroDeBloque);
	fprintf(archivoDeSeguimiento, "%s\n%f\n%f\n%f\n%f\n", "proporciones: ",p1,p2,p3,p4);
	fprintf(archivoDeSeguimiento, "%s (%d - %d) / %d\n","operacion:",nodo1->cantidadBloquesTotal,nodo1->cantidadBloquesLibres,nodo1->cantidadBloquesTotal);
	fprintf(archivoDeSeguimiento, "%s (%d - %d) / %d\n","operacion:",nodo2->cantidadBloquesTotal,nodo2->cantidadBloquesLibres,nodo2->cantidadBloquesTotal);
	fprintf(archivoDeSeguimiento, "%s (%d - %d) / %d\n","operacion:",nodo3->cantidadBloquesTotal,nodo3->cantidadBloquesLibres,nodo3->cantidadBloquesTotal);
	fprintf(archivoDeSeguimiento, "%s (%d - %d) / %d\n","operacion:",nodo4->cantidadBloquesTotal,nodo4->cantidadBloquesLibres,nodo4->cantidadBloquesTotal);
	fwrite(nodo1->nombre, strlen(nodo1->nombre), 1, archivoDeSeguimiento);
	fputs("\n",archivoDeSeguimiento);
	fwrite(nodo2->nombre, strlen(nodo2->nombre), 1, archivoDeSeguimiento);
	fputs("\n",archivoDeSeguimiento);
	*/
	printf("Se envio bloque a Nodo2 %d bytes\n",estado);

	almacenarBloquesEnEstructuraArchivo(estructuraArchivoAAlmacenar, nodo1, nodo2, bloque);
	free(head);
	liberarEstructuraBuffer(buffer1);
	liberarEstructuraBuffer(buffer2);
}

int cantidadDeBloquesDeUnArchivo(unsigned long long tamanio){
	float tamanioBloque = BLOQUE_SIZE;
	return ceil((float)tamanio / tamanioBloque);
}

void guardarTablaDeArchivo(Tarchivo * archivoAAlmacenar, char * rutaDestino){
	int index = obtenerIndexDeUnaRuta(rutaDestino);
	char * rutaArchivo = malloc(200);
	sprintf(rutaArchivo, "/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/metadata/archivos/%d/%s.%s", index, archivoAAlmacenar->nombreArchivoSinExtension, archivoAAlmacenar->extensionArchivo);
	FILE * archivo = fopen(rutaArchivo, "w+");
	fclose(archivo);
	almacenarEstructuraArchivoEnUnArchivo(archivoAAlmacenar, rutaArchivo);
	free(rutaArchivo);

}

void procesarArchivoBinario(Tarchivo * archivoAAlmacenar, char * archivoMapeado, TbloqueAEnviar * infoBloque){
	int cantidadDeBloquesDelArchivo = cantidadDeBloquesDeUnArchivo(archivoAAlmacenar->tamanioTotal);
	unsigned long long bytesFaltantesPorEnviar = archivoAAlmacenar->tamanioTotal;
	char * punteroAuxiliar = archivoMapeado;
	infoBloque->tamanio = BLOQUE_SIZE;

	while(cantidadDeBloquesDelArchivo != 1){
		memcpy(infoBloque->contenido, punteroAuxiliar, BLOQUE_SIZE);
		enviarBloque(infoBloque, archivoAAlmacenar);
		infoBloque->numeroDeBloque++;
		cantidadDeBloquesDelArchivo--;
		punteroAuxiliar += BLOQUE_SIZE;
		bytesFaltantesPorEnviar -= BLOQUE_SIZE;
	}

	//el ultimo bloque tiene tamaño <= que BLOQUE_SIZE (1024*1024)
	infoBloque->tamanio = bytesFaltantesPorEnviar;
	memcpy(infoBloque->contenido, punteroAuxiliar, bytesFaltantesPorEnviar);
	enviarBloque(infoBloque, archivoAAlmacenar);

}

void procesarArchivoCsv(Tarchivo * archivoAAlmacenar, char * archivoMapeado, TbloqueAEnviar* infoBloque){
	char * punteroAuxiliar = archivoMapeado;
	unsigned long long bytesFaltantesPorEnviar = archivoAAlmacenar->tamanioTotal;
	unsigned long long posicionUltimoBarraN = 0;
	unsigned long long bytesCopiados = 0;


	while(bytesFaltantesPorEnviar > 0){
		if(bytesFaltantesPorEnviar < BLOQUE_SIZE){
			memcpy(infoBloque->contenido,punteroAuxiliar,bytesFaltantesPorEnviar);
			infoBloque->tamanio = bytesFaltantesPorEnviar;
			bytesFaltantesPorEnviar = 0;
		}
		else {
			posicionUltimoBarraN = posicionUltimoBarraN + BLOQUE_SIZE;
			while(archivoMapeado[posicionUltimoBarraN] != '\n'){
				posicionUltimoBarraN--;
			}
			memcpy(infoBloque->contenido,punteroAuxiliar,posicionUltimoBarraN);
			infoBloque->tamanio = posicionUltimoBarraN - bytesCopiados;
			bytesCopiados = posicionUltimoBarraN;
		}
		bytesFaltantesPorEnviar-=bytesCopiados;
		//freir infoBloque->contenido en enviarBloque;
		//los tamaños varian según la posición del \n;
		enviarBloque(infoBloque, archivoAAlmacenar);
		punteroAuxiliar = archivoMapeado + posicionUltimoBarraN;
		infoBloque->numeroDeBloque++;
	}
}

int verificarDisponibilidadDeEspacioEnNodos(unsigned long long tamanioDelArchivoAGuardar){
	int tamanioEnMBDisponiblesEnNodos = sumarListasPorTamanioDatabin();
	//se multiplica por 2 por que se guarda 1 copia en otro nodo
	if(tamanioEnMBDisponiblesEnNodos * BLOQUE_SIZE < tamanioDelArchivoAGuardar * 2){
		return -1;
	}
	return 0;
}

int procesarArchivoSegunExtension(Tarchivo * archivoAAlmacenar, char * nombreArchivo){
	FILE * archivoOrigen = fopen(nombreArchivo, "r");
	int fd = fileno(archivoOrigen);
	char * archivoMapeado;
	unsigned long long tamanio = tamanioArchivo(archivoOrigen);
	archivoAAlmacenar->tamanioTotal = tamanio;
	TbloqueAEnviar * infoBloque = malloc(sizeof(TbloqueAEnviar));
	infoBloque->numeroDeBloque = 0;
	infoBloque->contenido = malloc(BLOQUE_SIZE);

	printf("El tamaño del archivo es: %llu\n", tamanio);

	if ((archivoMapeado = mmap(NULL, tamanio, PROT_READ, MAP_SHARED,	fd, 0)) == MAP_FAILED) {
		logAndExit("Error al hacer mmap");
	}

	fclose(archivoOrigen);
	close(fd);

	archivoAAlmacenar->bloques = malloc(sizeof(Tbloques) * cantidadDeBloquesDeUnArchivo(tamanio));

	printf("La cantidad de bloquees es: %d \n", cantidadDeBloquesDeUnArchivo(tamanio));

	if(tamanio == 0){
		puts("Error al almacenar archivo, está vacío");
		log_trace(logger, "Error al almacenar archivo, está vacío");
		liberarEstructuraBloquesAEnviar(infoBloque);
		return -1;
	}

	if(verificarDisponibilidadDeEspacioEnNodos(tamanio) == -1){
		puts("No hay suficiente espacio en los datanodes, intente con un archivo más chico");
		log_trace(logger, "No hay suficiente espacio en los datanodes, intente con un archivo más chico");
		liberarEstructuraBloquesAEnviar(infoBloque);
		return -1;
	}

	if(strcmp(archivoAAlmacenar->extensionArchivo, "csv") == 0){
		procesarArchivoCsv(archivoAAlmacenar, archivoMapeado, infoBloque);
	}
	else{
		procesarArchivoBinario(archivoAAlmacenar, archivoMapeado, infoBloque);
	}

	liberarEstructuraBloquesAEnviar(infoBloque);
	return 0;
}

void almacenarArchivo(char **palabras){
	//palabras[1] --> ruta archivo a almacenar
	//palabras[2] --> ruta de nuestro directorio
	char ** splitDeRuta = string_split(palabras[1], "/");
	char * nombreArchivoConExtension = obtenerUltimoElementoDeUnSplit(splitDeRuta);
	printf("El archivo a guardar es: %s\n", nombreArchivoConExtension);

	Tarchivo * archivoAAlmacenar = malloc(sizeof(Tarchivo));
	archivoAAlmacenar->nombreArchivoSinExtension = obtenerNombreDeArchivoSinExtension(nombreArchivoConExtension);
	puts("obtuvo el nombre del archivo sin extension");
	archivoAAlmacenar->extensionArchivo = obtenerExtensionDeUnArchivo(nombreArchivoConExtension);
	printf("El nombre del archivo es: %s\n", archivoAAlmacenar->nombreArchivoSinExtension);
	printf("La extensión es es: %s\n", archivoAAlmacenar->extensionArchivo);

	if(procesarArchivoSegunExtension(archivoAAlmacenar, palabras[1]) == -1){
		liberarPunteroDePunterosAChar(splitDeRuta);
		free(splitDeRuta);
		free(nombreArchivoConExtension);
		liberarTablaDeArchivo(archivoAAlmacenar);
		return;
	}

	guardarTablaDeArchivo(archivoAAlmacenar, palabras[2]);
	liberarPunteroDePunterosAChar(splitDeRuta);
	free(splitDeRuta);
	free(nombreArchivoConExtension);
	liberarTablaDeArchivo(archivoAAlmacenar);
}

Tnodo * inicializarNodo(TpackInfoBloqueDN * infoBloqueRecibido, int fileDescriptor){
	Tnodo * nuevoNodo = malloc(sizeof(Tnodo));
	nuevoNodo->fd = fileDescriptor;
	nuevoNodo->cantidadBloquesTotal = infoBloqueRecibido->databinEnMB;
	nuevoNodo->cantidadBloquesLibres = infoBloqueRecibido->databinEnMB;
	nuevoNodo->primerBloqueLibreBitmap = 0;
	puts(infoBloqueRecibido->nombreNodo);
	nuevoNodo->nombre = strdup(infoBloqueRecibido->nombreNodo);
	nuevoNodo->bitmap = crearBitmap(infoBloqueRecibido->databinEnMB);
	return nuevoNodo;
}

TpackInfoBloqueDN * recvInfoNodo(int socketFS){
	int estado;
	TpackInfoBloqueDN * infoBloque = malloc(sizeof(TpackInfoBloqueDN));
	char * nombreNodo;
	char * ipNodo;
	char * puertoNodo;

	//Recibo el tamaño del nombre del nodo
	if ((estado = recv(socketFS, &infoBloque->tamanioNombre, sizeof(int), 0)) == -1) {
		logAndExit("Error al recibir el tamanio del nombre del nodo");
		}
	printf("Para el tamaño del nombre recibi %d bytes\n", estado);
	nombreNodo = malloc(infoBloque->tamanioNombre);

	//Recibo el nombre del nodo
	if ((estado = recv(socketFS, nombreNodo, infoBloque->tamanioNombre, 0)) == -1) {
		logAndExit("Error al recibir el nombre del nodo");
		}

	printf("Para el nombre del nodo recibi %d bytes\n", estado);

	//Recibo el tamanio de la ip del nodo
	if ((estado = recv(socketFS, &infoBloque->tamanioIp, sizeof(int), 0)) == -1) {
		logAndExit("Error al recibir el tamanio del ip del nodo");
		}
	printf("Para el tamaño de la ip recibi %d bytes\n", estado);

	ipNodo = malloc(infoBloque->tamanioIp);

	//Recibo la ip del nodo
	if ((estado = recv(socketFS, ipNodo, infoBloque->tamanioIp, 0)) == -1) {
		logAndExit("Error al recibir el ip del nodo");
		}

	printf("Para el la ip recibi %d bytes\n", estado);

	//Recibo el tamanio del puerto del nodo
	if ((estado = recv(socketFS, &infoBloque->tamanioPuerto, sizeof(int), 0)) == -1) {
		logAndExit("Error al recibir el tamanio del puerto del nodo");
		}
	printf("Para el tamaño del puerto recibi %d bytes\n", estado);

	puertoNodo = malloc(infoBloque->tamanioPuerto);

	//Recibo el puerto del nodo
	if ((estado = recv(socketFS, puertoNodo, infoBloque->tamanioPuerto, 0)) == -1) {
		logAndExit("Error al recibir el puerto del nodo");
		}

	//Recibo el databin en MB
	if ((estado = recv(socketFS, &infoBloque->databinEnMB, sizeof(int), 0)) == -1) {
		logAndExit("Error al recibir el tamanio del databin");
		}

	printf("Para el tamanio del databin recibi %d bytes\n", estado);

	 infoBloque = desempaquetarInfoNodo(infoBloque, nombreNodo, ipNodo, puertoNodo);

	 free(nombreNodo);
	 free(ipNodo);
	 free(puertoNodo);

	 puts("desempaqueta la info del nodo");
	 return infoBloque;
}

int getMD5(char**palabras){
	/*Tdirectorio * directorio;
	char ** splitDeRuta = string_split(palabras[1], "/");
	char * nombreArchivoConExtension = obtenerUltimoElementoDeUnSplit(splitDeRuta);
	printf("El archivo del que queremos su MD5 es: %s\n", nombreArchivoConExtension);

	Tarchivo * archivo = malloc(sizeof(Tarchivo));
	archivo->nombreArchivoSinExtension = obtenerNombreDeArchivoSinExtension(nombreArchivoConExtension);
	archivo->extensionArchivo = obtenerExtensionDeUnArchivo(nombreArchivoConExtension);

	char* ruta_temporal = string_duplicate("/tmp/");
		string_append(&ruta_temporal, archivo->nombreArchivoSinExtension);
		string_append(&ruta_temporal, ".");
		string_append(&ruta_temporal, archivo->extensionArchivo);
		copiar_archivo_mdfs_a_local(palabras[1], ruta_temporal); //FALTA ESTO

		char* comando = string_duplicate("md5sum ");
		string_append(&comando, ruta_temporal);
		system(comando);

		free(comando);
		free(ruta_temporal);*/
		return 0;
}
