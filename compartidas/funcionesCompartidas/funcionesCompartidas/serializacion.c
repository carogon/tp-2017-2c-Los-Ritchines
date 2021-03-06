#include "serializacion.h"


char *serializeInfoBloque(Theader head, TpackInfoBloque * infoBloque, int *pack_size){

	char *bytes_serial;

	int espacioPackSize = sizeof(int);
	int espacioEnteros = sizeof(int) * 8;
	int espaciosVariables = infoBloque->tamanioIp+infoBloque->tamanioNombre+infoBloque->nombreTemporalLen+infoBloque->tamanioPuerto;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioEnteros+espaciosVariables;

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoBloque->idTarea, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoBloque->tamanioNombre, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoBloque->nombreNodo, infoBloque->tamanioNombre);
	*pack_size += infoBloque->tamanioNombre;



	memcpy(bytes_serial + *pack_size, &infoBloque->tamanioIp, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoBloque->ipWorker, infoBloque->tamanioIp);
	*pack_size += infoBloque->tamanioIp;



	memcpy(bytes_serial + *pack_size, &infoBloque->tamanioPuerto, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoBloque->puertoWorker, infoBloque->tamanioPuerto);
	*pack_size += infoBloque->tamanioPuerto;


	memcpy(bytes_serial + *pack_size, &infoBloque->bloqueDelArchivo, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoBloque->bloqueDelDatabin, sizeof(int));
	*pack_size += sizeof(int);


	memcpy(bytes_serial + *pack_size, &infoBloque->bytesOcupados, sizeof(int));
	*pack_size += sizeof(int);


	memcpy(bytes_serial + *pack_size, &infoBloque->nombreTemporalLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoBloque->nombreTemporal, infoBloque->nombreTemporalLen);
	*pack_size += infoBloque->nombreTemporalLen;


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}


TpackInfoBloque *deserializeInfoBloque(char *bytes_serial){

	int off;
	TpackInfoBloque *infoBloque;

	if ((infoBloque = malloc(sizeof(TpackInfoBloque))) == NULL) {
		log_error(logError,	"No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	off = 0;

	memcpy(&infoBloque->idTarea, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	memcpy(&infoBloque->tamanioNombre, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	if ((infoBloque->nombreNodo = malloc(infoBloque->tamanioNombre)) == NULL) {
		log_error(logError,"error malloc deser info bloque ");

		return NULL;
	}

	memcpy(infoBloque->nombreNodo, bytes_serial + off, infoBloque->tamanioNombre);
	off += infoBloque->tamanioNombre;

	memcpy(&infoBloque->tamanioIp, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	if ((infoBloque->ipWorker = malloc(infoBloque->tamanioIp)) == NULL) {

		log_error(logError,"error malloc deser info bl");
		return NULL;
	}

	memcpy(infoBloque->ipWorker, bytes_serial + off, infoBloque->tamanioIp);
	off += infoBloque->tamanioIp;

	memcpy(&infoBloque->tamanioPuerto, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	if ((infoBloque->puertoWorker = malloc(infoBloque->tamanioPuerto)) == NULL) {
		log_error(logError,"error malloc deser ingo bloqe ");

		return NULL;
	}

	memcpy(infoBloque->puertoWorker, bytes_serial + off, infoBloque->tamanioPuerto);
	off += infoBloque->tamanioPuerto;

	memcpy(&infoBloque->bloqueDelArchivo, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	memcpy(&infoBloque->bloqueDelDatabin, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	memcpy(&infoBloque->bytesOcupados, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	memcpy(&infoBloque->nombreTemporalLen, bytes_serial + off, sizeof(int));
	off += sizeof(int);

	if ((infoBloque->nombreTemporal = malloc(infoBloque->nombreTemporalLen)) == NULL) {
		log_error(logError,"error malloc deser info bl");
		return NULL;
	}

	memcpy(infoBloque->nombreTemporal, bytes_serial + off, infoBloque->nombreTemporalLen);
	off += infoBloque->tamanioNombre;

	return infoBloque;
}

char *serializeBytes(Theader head, char* buffer, int buffer_size, int *pack_size){

	char *bytes_serial;
	if ((bytes_serial = malloc(HEAD_SIZE + sizeof(int) + sizeof(int) + buffer_size)) == NULL){


		log_error(logError, "No se pudo mallocar espacio para paquete de bytes");

		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &buffer_size, sizeof buffer_size);
	*pack_size += sizeof (int);
	memcpy(bytes_serial + *pack_size, buffer, buffer_size);
	*pack_size += buffer_size;

	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}


TpackBytes *deserializeBytes(char *bytes_serial){

	int off;
	TpackBytes *pbytes;

	if ((pbytes = malloc(sizeof (TpackBytes))) == NULL){
		log_error(logError,"error malloc deser byetes");
		return NULL;
	}

	off = 0;
	memcpy(&pbytes->bytelen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((pbytes->bytes = malloc(pbytes->bytelen)) == NULL){
		log_error(logError,"errormalloc deser bytes");
		return NULL;
	}

	memcpy(pbytes->bytes, bytes_serial + off, pbytes->bytelen);
	off += pbytes->bytelen;

	return pbytes;
}

TpackSrcCode *readFileIntoPack(Tproceso sender, char* ruta){

	FILE *file = fopen(ruta, "rb");
	TpackSrcCode *src_code = malloc(sizeof (TpackSrcCode));
	src_code->head.tipo_de_proceso = sender;
	src_code->head.tipo_de_mensaje = SRC_CODE;

	unsigned long fileSize = fsize(file) + 1 ; // + 1 para el '\0'
	src_code->bytelen = fileSize;
	src_code->bytes = malloc(src_code->bytelen);
	fread(src_code->bytes, src_code->bytelen, 1, file);
	fclose(file);
	// ponemos un '\0' al final porque es probablemente mandatorio para que se lea, send'ee y recv'ee bien despues
	src_code->bytes[src_code->bytelen - 1] = '\0';

	return src_code;
}

unsigned long fsize(FILE* f){

    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long) ftell(f);
    fseek(f, 0, SEEK_SET);
    return len;

}

Tbuffer *empaquetarBloque(Theader * head, TbloqueAEnviar* bloque, Tnodo* nodo){

	Tbuffer *buffer = malloc(sizeof(Tbuffer));
	buffer->tamanio = (HEAD_SIZE + sizeof(int) + sizeof(unsigned long long) + bloque->tamanio);
	buffer->buffer = malloc(buffer->tamanio);
	int primerBloqueLibre = obtenerBloqueDisponible(nodo);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &primerBloqueLibre, sizeof(int));
	p += sizeof(int);
	memcpy(p, &bloque->tamanio, sizeof(unsigned long long));
	p += sizeof(unsigned long long);
	memcpy(p, bloque->contenido, bloque->tamanio);
	p += bloque->tamanio;


	return buffer;
}

Tbuffer * empaquetarInfoNodo(TpackInfoBloqueDN * infoBloque){

		Tbuffer *buffer = malloc(sizeof(Tbuffer));
		int espacioEnteros = sizeof(int) * 4;
		int espaciosVariables = infoBloque->tamanioIp + infoBloque->tamanioPuerto + infoBloque->tamanioNombre;
		buffer->tamanio = HEAD_SIZE + espacioEnteros + espaciosVariables;
		buffer->buffer = malloc(buffer->tamanio);

		char * p = buffer->buffer;

		memcpy(p, &infoBloque->head, sizeof(infoBloque->head));
		p += sizeof(infoBloque->head);
		memcpy(p, &infoBloque->tamanioNombre, sizeof(int));
		p += sizeof(int);
		memcpy(p, infoBloque->nombreNodo, infoBloque->tamanioNombre);
		p += infoBloque->tamanioNombre;
		memcpy(p, &infoBloque->tamanioIp, sizeof(int));
		p += sizeof(int);
		memcpy(p, infoBloque->ipNodo, infoBloque->tamanioIp);
		p += infoBloque->tamanioIp;
		memcpy(p, &infoBloque->tamanioPuerto, sizeof(int));
		p += sizeof(int);
		memcpy(p, infoBloque->puertoNodo, infoBloque->tamanioPuerto);
		p += infoBloque->tamanioPuerto;
		memcpy(p, &infoBloque->databinEnMB, sizeof(int));
		p += sizeof(int);

		return buffer;

}

TpackInfoBloqueDN * desempaquetarInfoNodo(TpackInfoBloqueDN * infoBloque, char * nombreNodo, char * ipNodo, char * puertoNodo){

	infoBloque->nombreNodo = malloc(infoBloque->tamanioNombre);
	infoBloque->ipNodo = malloc(infoBloque->tamanioIp);
	infoBloque->puertoNodo = malloc(infoBloque->tamanioPuerto);

	memcpy(infoBloque->nombreNodo, nombreNodo, infoBloque->tamanioNombre);
	memcpy(infoBloque->ipNodo, ipNodo, infoBloque->tamanioIp);
	memcpy(infoBloque->puertoNodo, puertoNodo, infoBloque->tamanioPuerto);

	return infoBloque;

}

char *serializarInfoTransformacionMasterWorker(Theader head,int nroBloque, int bytesOcupadosBloque,int nombreTemporalLen ,char* nombreTemporal, int *pack_size){

	char *bytes_serial;
	if ((bytes_serial = malloc(HEAD_SIZE + sizeof(int) + sizeof(int)*3 + nombreTemporalLen)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &nroBloque, sizeof (int));
	*pack_size += sizeof (int);

	memcpy(bytes_serial + *pack_size, &bytesOcupadosBloque, sizeof (int));
	*pack_size += sizeof (int);

	memcpy(bytes_serial + *pack_size, &nombreTemporalLen, sizeof (int));
	*pack_size += sizeof (int);

	memcpy(bytes_serial + *pack_size, nombreTemporal, nombreTemporalLen);
	*pack_size += nombreTemporalLen;

	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}


TpackDatosTransformacion *deserializarInfoTransformacionMasterWorker(char *bytes_serial){

	int off;
	TpackDatosTransformacion *datosTransf;

	if ((datosTransf = malloc(sizeof (TpackDatosTransformacion))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete datos transf.");
		return NULL;
	}

	off = 0;
	memcpy(&datosTransf->nroBloque, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&datosTransf->bytesOcupadosBloque, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&datosTransf->nombreTemporalLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((datosTransf->nombreTemporal = malloc(datosTransf->nombreTemporalLen)) == NULL){
		log_error(logError,"error malloc des info trans m w");
		return NULL;
	}

	memcpy(datosTransf->nombreTemporal, bytes_serial + off, datosTransf->nombreTemporalLen);
	off += datosTransf->nombreTemporalLen;

	return datosTransf;
}

int enviarHeaderYValor(Theader head, int valorAEnviar,int socketDestino){

	char *bytes_serial;
	int pack_size;
	int estado;
	if ((bytes_serial = malloc(HEAD_SIZE + sizeof(int) + sizeof(int))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return -1;
	}

	pack_size = 0;
	memcpy(bytes_serial + pack_size, &head, HEAD_SIZE);
	pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	pack_size += sizeof(int);

	memcpy(bytes_serial + pack_size, &valorAEnviar, sizeof (int));
	pack_size += sizeof (int);


	memcpy(bytes_serial + HEAD_SIZE, &pack_size, sizeof(int));

	if ((estado = send(socketDestino, bytes_serial, pack_size, 0)) == -1){
		logErrorAndExit("Fallo al enviar el header.");
	}

	free(bytes_serial);
	return estado;
}

int recibirValor(int fd){
	int valorRet;
	char* buffer=recvGeneric(fd);
	memcpy(&valorRet,buffer, sizeof (int));
	free(buffer);
	return valorRet;
}


char *serializeInfoReduccionLocal(Theader head, TreduccionLocal * infoReduccion, int *pack_size){
	char *bytes_serial;
	int i;
	int espacioPackSize = sizeof(int);
	int espacioEnteros = sizeof(int) * 7;
	int espacioLista=0;

	int sizeLista = list_size(infoReduccion->listaTemporalesTransformacion);
	for(i=0;i< sizeLista;i++){
		TreduccionLista * aux = list_get(infoReduccion->listaTemporalesTransformacion,i);
		espacioLista += aux->nombreTemporalLen;
	}
	espacioLista+=sizeof(int)*sizeLista;


	int espaciosVariables = infoReduccion->ipLen+infoReduccion->puertoLen+infoReduccion->nombreNodoLen+infoReduccion->tempRedLen+espacioLista;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioEnteros+espaciosVariables;

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoReduccion->job, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoReduccion->idTarea, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoReduccion->nombreNodoLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoReduccion->nombreNodo, infoReduccion->nombreNodoLen);
	*pack_size += infoReduccion->nombreNodoLen;

	memcpy(bytes_serial + *pack_size, &infoReduccion->ipLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoReduccion->ipNodo, infoReduccion->ipLen);
	*pack_size += infoReduccion->ipLen;

	memcpy(bytes_serial + *pack_size, &infoReduccion->puertoLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoReduccion->puertoNodo, infoReduccion->puertoLen);
	*pack_size += infoReduccion->puertoLen;

	memcpy(bytes_serial + *pack_size, &infoReduccion->tempRedLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoReduccion->tempRed, infoReduccion->tempRedLen);
	*pack_size += infoReduccion->tempRedLen;

	memcpy(bytes_serial + *pack_size, &infoReduccion->listaSize, sizeof(int));
	*pack_size += sizeof(int);

	for(i=0;i<sizeLista;i++){
		TreduccionLista * aux = list_get(infoReduccion->listaTemporalesTransformacion,i);
		memcpy(bytes_serial + *pack_size, &aux->nombreTemporalLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, aux->nombreTemporal, aux->nombreTemporalLen);
		*pack_size += aux->nombreTemporalLen;
	}


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}

TreduccionLocal *deserializeInfoReduccionLocal(char *bytes_serial){

	int off;
	TreduccionLocal *infoReduccion;

	if ((infoReduccion = malloc(sizeof (TreduccionLocal))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	off = 0;

	memcpy(&infoReduccion->job, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&infoReduccion->idTarea, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&infoReduccion->nombreNodoLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((infoReduccion->nombreNodo = malloc(infoReduccion->nombreNodoLen)) == NULL){
		log_error(logError,"error malloc des inf rl");
		return NULL;
	}

	memcpy(infoReduccion->nombreNodo, bytes_serial + off, infoReduccion->nombreNodoLen);
	off += infoReduccion->nombreNodoLen;



	memcpy(&infoReduccion->ipLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((infoReduccion->ipNodo = malloc(infoReduccion->ipLen)) == NULL){
		log_error(logError,"error malloc des irl ");
		return NULL;
	}

	memcpy(infoReduccion->ipNodo, bytes_serial + off, infoReduccion->ipLen);
	off += infoReduccion->ipLen;


	memcpy(&infoReduccion->puertoLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((infoReduccion->puertoNodo = malloc(infoReduccion->puertoLen)) == NULL){
		log_error(logError,"error malloc des irl");
		return NULL;
	}

	memcpy(infoReduccion->puertoNodo, bytes_serial + off, infoReduccion->puertoLen);
	off += infoReduccion->puertoLen;




	memcpy(&infoReduccion->tempRedLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((infoReduccion->tempRed = malloc(infoReduccion->tempRedLen)) == NULL){
		log_error(logError,"error malloc des irl");
		return NULL;
	}

	memcpy(infoReduccion->tempRed, bytes_serial + off, infoReduccion->tempRedLen);
	off += infoReduccion->tempRedLen;


	memcpy(&infoReduccion->listaSize, bytes_serial + off, sizeof (int));
	off += sizeof (int);


	int i;
	t_list *listaTemporales = list_create();
	for(i=0;i<infoReduccion->listaSize;i++){

		TreduccionLista *aux = malloc(sizeof (TreduccionLista));
		memcpy(&aux->nombreTemporalLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((aux->nombreTemporal = malloc(aux->nombreTemporalLen)) == NULL){
			log_error(logError,"error malloc des irl ");
			return NULL;
		}

		memcpy(aux->nombreTemporal, bytes_serial + off, aux->nombreTemporalLen);
		off += aux->nombreTemporalLen;
		list_add(listaTemporales,aux);
	}

	infoReduccion->listaTemporalesTransformacion=listaTemporales;


	return infoReduccion;
}


char * serializarListaNombresTemporales(Theader head,t_list * listaNombres,int *pack_size){
	char *bytes_serial;
	int i;
	int espacioPackSize = sizeof(int);
	int espacioListSize = sizeof(int);

	int sizeLista = list_size(listaNombres);
	int espacioEnteros = sizeof(int) * sizeLista;
	int espaciosVariables=0;
	for(i=0;i< sizeLista;i++){
		TreduccionLista * aux = list_get(listaNombres,i);
		espaciosVariables += aux->nombreTemporalLen;
	}
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioListSize+espacioEnteros+espaciosVariables;

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	// hacemos lugar para el lista_size
	*pack_size += sizeof(int);

	for(i=0;i<sizeLista;i++){
		TreduccionLista * aux = list_get(listaNombres,i);
		memcpy(bytes_serial + *pack_size, &aux->nombreTemporalLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, aux->nombreTemporal, aux->nombreTemporalLen);
		*pack_size += aux->nombreTemporalLen;
	}

	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));
	memcpy(bytes_serial + HEAD_SIZE+espacioPackSize, &sizeLista, sizeof(int));

	return bytes_serial;
}

t_list * deserializarListaNombresTemporales(char * bytes_serial){
	t_list * listaRet=list_create();

	int off;
	off = 0;
	int listaSize;

	memcpy(&listaSize, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	int i;
	for(i=0;i<listaSize;i++){

		TreduccionLista *aux = malloc(sizeof (TreduccionLista));
		memcpy(&aux->nombreTemporalLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);


		if ((aux->nombreTemporal = malloc(aux->nombreTemporalLen)) == NULL){
			char * mensaje = malloc(200);

			sprintf(mensaje, "No se pudieron mallocar %d bytes al Paquete De Bytes.", aux->nombreTemporalLen);
			log_error(logError, mensaje);
			free(mensaje);
			return NULL;
		}

		memcpy(aux->nombreTemporal, bytes_serial + off, aux->nombreTemporalLen);
		off += aux->nombreTemporalLen;
		list_add(listaRet,aux);
	}



	return listaRet;
}




char *serializarInfoReduccionLocalMasterWorker(Theader head,int nombreTemporalReduccionLen,char * nombreTemporalReduccion,t_list * listaTemporales, int *pack_size){

	char *bytes_serial;

	int i;
	int espacioPackSize = sizeof(int);
	int espacioListSize = sizeof(int);
	int espacioNombreTemporalLen = sizeof(int);
	int sizeLista = list_size(listaTemporales);

	int espaciosVariables=0;
	for(i=0;i< sizeLista;i++){
		TreduccionLista * aux = list_get(listaTemporales,i);
		espaciosVariables += aux->nombreTemporalLen;
	}
	espaciosVariables += sizeof(int)*sizeLista;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioListSize+espacioNombreTemporalLen+nombreTemporalReduccionLen+espaciosVariables;
	//printf("Espacio a mallocar: %d\n",espacioAMallocar);

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}


	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &nombreTemporalReduccionLen, sizeof (int));
	*pack_size += sizeof (int);
	memcpy(bytes_serial + *pack_size, nombreTemporalReduccion, nombreTemporalReduccionLen);
	*pack_size += nombreTemporalReduccionLen;


	memcpy(bytes_serial + *pack_size, &sizeLista, sizeof (int));
	*pack_size += sizeof (int);

	//printf("EN COMPARTIDAS; SIZE LISTA: %d\n",sizeLista);
	for(i=0;i<sizeLista;i++){
		TreduccionLista * aux = list_get(listaTemporales,i);
		memcpy(bytes_serial + *pack_size, &aux->nombreTemporalLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, aux->nombreTemporal, aux->nombreTemporalLen);
		*pack_size += aux->nombreTemporalLen;
	}


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	//printf("Pack size:serializarInfoReduccionLocalMasterWorker2 %d\n",*pack_size);
	return bytes_serial;
}


TinfoReduccionLocalMasterWorker *deserializarInfoReduccionLocalMasterWorker(char *bytes_serial){

	int off;
	TinfoReduccionLocalMasterWorker *datosReduccion;

	if ((datosReduccion = malloc(sizeof (TinfoReduccionLocalMasterWorker))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete datos reduccion.");
		return NULL;
	}

	off = 0;
	memcpy(&datosReduccion->nombreTempReduccionLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((datosReduccion->nombreTempReduccion = malloc(datosReduccion->nombreTempReduccionLen)) == NULL){
		char * mensaje = malloc(200);

		sprintf(mensaje, "No se pudieron mallocar %d bytes al Paquete De Bytes.", datosReduccion->nombreTempReduccionLen);
		log_error(logError, mensaje);
		free(mensaje);
		return NULL;
	}

	memcpy(datosReduccion->nombreTempReduccion, bytes_serial + off, datosReduccion->nombreTempReduccionLen);
	off += datosReduccion->nombreTempReduccionLen;



	memcpy(&datosReduccion->listaSize, bytes_serial + off, sizeof (int));
	off += sizeof (int);


	int i;
	t_list * listaRet = list_create();
	for(i=0;i<datosReduccion->listaSize;i++){

		TreduccionLista *aux = malloc(sizeof aux);
		memcpy(&aux->nombreTemporalLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);


		if ((aux->nombreTemporal = malloc(aux->nombreTemporalLen)) == NULL){
			log_error(logError,"error malloc des irlmw");
			return NULL;
		}

		memcpy(aux->nombreTemporal, bytes_serial + off, aux->nombreTemporalLen);
		off += aux->nombreTemporalLen;
		list_add(listaRet,aux);
	}

	datosReduccion->listaTemporales=listaRet;



	return datosReduccion;
}

Tbuffer * empaquetarBytesMasInt(Theader* head, char * bytes, int numero) {
	Tbuffer * buffer = malloc(sizeof(Tbuffer));
	int tamanio = strlen(bytes) + 1;
	buffer->tamanio = (HEAD_SIZE + sizeof(int) + sizeof(int) + tamanio);
	buffer->buffer = malloc(buffer->tamanio);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &numero, sizeof(int));
	p += sizeof(int);
	memcpy(p, &buffer->tamanio, sizeof(int));
	p += sizeof(int);
	memcpy(p, bytes, buffer->tamanio);
	p += buffer->tamanio;

	return buffer;
}

Tbuffer *empaquetarBytes(Theader * head, char * bytes){

	Tbuffer * buffer = malloc(sizeof(Tbuffer));
	int tamanio = strlen(bytes) +1;
	buffer->tamanio = (HEAD_SIZE + sizeof(int) + tamanio);
	buffer->buffer = malloc(buffer->tamanio);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &buffer->tamanio, sizeof(int));
	p += sizeof(int);
	memcpy(p, bytes, buffer->tamanio);
	p += buffer->tamanio;

	return buffer;
}

Tbuffer * empaquetarInt(Theader * head, int numero){

	Tbuffer * buffer = malloc(sizeof(Tbuffer));

	buffer->tamanio = (HEAD_SIZE + sizeof(int));
	buffer->buffer = malloc(buffer->tamanio);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &numero, sizeof(int));
	p += sizeof(int);

	return buffer;
}

Tbuffer * empaquetarPeticionBloque(Theader* head, int nroBloque, unsigned long long tamanioBloque){
	Tbuffer* buffer = malloc(sizeof(Tbuffer));
	buffer->tamanio = (HEAD_SIZE + sizeof(int) + sizeof(unsigned long long));
	buffer->buffer = malloc(buffer->tamanio);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &nroBloque, sizeof(int));
	p += sizeof(int);
	memcpy(p, &tamanioBloque, sizeof(unsigned long long));

	return buffer;
}

Tbuffer * empaquetarBloqueConNBytes(Theader* head, unsigned long long int tamanio, char* bloque, int nroBloque){
	Tbuffer* buffer = malloc(sizeof(Tbuffer));
	buffer->tamanio = (HEAD_SIZE + sizeof(unsigned long long int) + tamanio + sizeof(int));
	buffer->buffer = malloc(buffer->tamanio);

	char * p = buffer->buffer;
	memcpy(p, head, sizeof(*head));
	p += sizeof(*head);
	memcpy(p, &tamanio, sizeof(unsigned long long int));
	p += sizeof(unsigned long long int);
	memcpy(p, bloque, tamanio);
	p += tamanio;
	memcpy(p, &nroBloque, sizeof(int));

	return buffer;
}




char *serializeInfoReduccionGlobal(Theader head, TreduccionGlobal * infoReduccionGlobal, int *pack_size){
	char *bytes_serial;
	int i;
	int espacioPackSize = sizeof(int);
	int espacioEnteros = sizeof(int) * 4;
	int espacioLista=0;

	int sizeLista = infoReduccionGlobal->listaNodosSize;
	for(i=0;i< sizeLista;i++){
		TinfoNodoReduccionGlobal * infoAux = list_get(infoReduccionGlobal->listaNodos,i);
		espacioLista += sizeof(int)*5;
		espacioLista+=infoAux->ipNodoLen;
		espacioLista+=infoAux->puertoNodoLen;
		espacioLista+=infoAux->nombreNodoLen;
		espacioLista+=infoAux->temporalReduccionLen;
	}



	int espaciosVariables = infoReduccionGlobal->tempRedGlobalLen;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioEnteros+espacioLista+espaciosVariables;
	//printf("Espacio a mallocar %d \n",espacioAMallocar);
	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoReduccionGlobal->job, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoReduccionGlobal->idTarea, sizeof(int));
	*pack_size += sizeof(int);


	memcpy(bytes_serial + *pack_size, &infoReduccionGlobal->tempRedGlobalLen, sizeof(int));
	*pack_size += sizeof(int);
	memcpy(bytes_serial + *pack_size, infoReduccionGlobal->tempRedGlobal, infoReduccionGlobal->tempRedGlobalLen);
	*pack_size += infoReduccionGlobal->tempRedGlobalLen;


	memcpy(bytes_serial + *pack_size, &sizeLista, sizeof(int));
	*pack_size += sizeof(int);

	for(i=0;i<sizeLista;i++){
		TinfoNodoReduccionGlobal * infoAux = list_get(infoReduccionGlobal->listaNodos,i);

		memcpy(bytes_serial + *pack_size, &infoAux->nombreNodoLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, infoAux->nombreNodo, infoAux->nombreNodoLen);
		*pack_size += infoAux->nombreNodoLen;

		memcpy(bytes_serial + *pack_size, &infoAux->ipNodoLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, infoAux->ipNodo, infoAux->ipNodoLen);
		*pack_size += infoAux->ipNodoLen;

		memcpy(bytes_serial + *pack_size, &infoAux->puertoNodoLen, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, infoAux->puertoNodo, infoAux->puertoNodoLen);
		*pack_size += infoAux->puertoNodoLen;


		memcpy(bytes_serial + *pack_size, &infoAux->temporalReduccionLen , sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, infoAux->temporalReduccion, infoAux->temporalReduccionLen);
		*pack_size += infoAux->temporalReduccionLen;

		memcpy(bytes_serial + *pack_size, &infoAux->nodoEncargado , sizeof(int));
		*pack_size += sizeof(int);

	}


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));


	return bytes_serial;
}

TreduccionGlobal *deserializeInfoReduccionGlobal(char *bytes_serial){

	int off;
	TreduccionGlobal *infoReduccionGlobal;

	if ((infoReduccionGlobal = malloc(sizeof (TreduccionGlobal))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	off = 0;

	memcpy(&infoReduccionGlobal->job, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&infoReduccionGlobal->idTarea, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	memcpy(&infoReduccionGlobal->tempRedGlobalLen, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	if ((infoReduccionGlobal->tempRedGlobal = malloc(infoReduccionGlobal->tempRedGlobalLen)) == NULL){
		log_error(logError,"error malloc des irg");
		return NULL;
	}
	memcpy(infoReduccionGlobal->tempRedGlobal, bytes_serial + off, infoReduccionGlobal->tempRedGlobalLen);
	off += infoReduccionGlobal->tempRedGlobalLen;


	memcpy(&infoReduccionGlobal->listaNodosSize, bytes_serial + off, sizeof (int));
	off += sizeof (int);

	infoReduccionGlobal->listaNodos=list_create();

	int i;
	t_list *listaInfoNodos = list_create();
	for(i=0;i<infoReduccionGlobal->listaNodosSize;i++){

		TinfoNodoReduccionGlobal *infoAux = malloc(sizeof (TinfoNodoReduccionGlobal));

		memcpy(&infoAux->nombreNodoLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);
		if ((infoAux->nombreNodo = malloc(infoAux->nombreNodoLen)) == NULL){
			log_error(logError,"error malloc desirg ");
			return NULL;
		}
		memcpy(infoAux->nombreNodo, bytes_serial + off, infoAux->nombreNodoLen);
		off += infoAux->nombreNodoLen;


		memcpy(&infoAux->ipNodoLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);
		if ((infoAux->ipNodo = malloc(infoAux->ipNodoLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(infoAux->ipNodo, bytes_serial + off, infoAux->ipNodoLen);
		off += infoAux->ipNodoLen;



		memcpy(&infoAux->puertoNodoLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);
		if ((infoAux->puertoNodo = malloc(infoAux->puertoNodoLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(infoAux->puertoNodo, bytes_serial + off, infoAux->puertoNodoLen);
		off += infoAux->puertoNodoLen;




		memcpy(&infoAux->temporalReduccionLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);
		if ((infoAux->temporalReduccion = malloc(infoAux->temporalReduccionLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(infoAux->temporalReduccion, bytes_serial + off, infoAux->temporalReduccionLen);
		off += infoAux->temporalReduccionLen;

		memcpy(&infoAux->nodoEncargado, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		list_add(listaInfoNodos,infoAux);
	}


	infoReduccionGlobal->listaNodos=listaInfoNodos;


	return infoReduccionGlobal;
}

char *serializeInfoAlmacenadoFinal(Theader head, TinfoAlmacenadoFinal * infoAlmacenado, int *pack_size){

	char *bytes_serial;

	int espacioPackSize = sizeof(int);
	int espacioEnteros = sizeof(int) * 5;
	int espaciosVariables = infoAlmacenado->ipNodoLen+infoAlmacenado->nombreTempReduccionLen+infoAlmacenado->puertoNodoLen;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioEnteros+espaciosVariables;

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoAlmacenado->idTarea, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoAlmacenado->job, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoAlmacenado->ipNodoLen, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoAlmacenado->ipNodo, infoAlmacenado->ipNodoLen);
	*pack_size += infoAlmacenado->ipNodoLen;



	memcpy(bytes_serial + *pack_size, &infoAlmacenado->puertoNodoLen, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoAlmacenado->puertoNodo, infoAlmacenado->puertoNodoLen);
	*pack_size += infoAlmacenado->puertoNodoLen;



	memcpy(bytes_serial + *pack_size, &infoAlmacenado->nombreTempReduccionLen, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoAlmacenado->nombreTempReduccion, infoAlmacenado->nombreTempReduccionLen);
	*pack_size += infoAlmacenado->nombreTempReduccionLen;


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}


TinfoAlmacenadoFinal *deserializeInfoAlmacenadoFinal(char *bytes_serial){

	int off;
	TinfoAlmacenadoFinal *infoAlmacenado;

		if ((infoAlmacenado = malloc(sizeof (TinfoAlmacenadoFinal))) == NULL){
			log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
			return NULL;
		}

		off = 0;

		memcpy(&infoAlmacenado->idTarea, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		memcpy(&infoAlmacenado->job, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		memcpy(&infoAlmacenado->ipNodoLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((infoAlmacenado->ipNodo = malloc(infoAlmacenado->ipNodoLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}

		memcpy(infoAlmacenado->ipNodo, bytes_serial + off, infoAlmacenado->ipNodoLen);
		off += infoAlmacenado->ipNodoLen;



		memcpy(&infoAlmacenado->puertoNodoLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((infoAlmacenado->puertoNodo = malloc(infoAlmacenado->puertoNodoLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}

		memcpy(infoAlmacenado->puertoNodo, bytes_serial + off, infoAlmacenado->puertoNodoLen);
		off += infoAlmacenado->puertoNodoLen;





		memcpy(&infoAlmacenado->nombreTempReduccionLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((infoAlmacenado->nombreTempReduccion = malloc(infoAlmacenado->nombreTempReduccionLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}

		memcpy(infoAlmacenado->nombreTempReduccion, bytes_serial + off, infoAlmacenado->nombreTempReduccionLen);
		off += infoAlmacenado->nombreTempReduccionLen;

		return infoAlmacenado;
}

char *serializeInfoAlmacenadoFinalMasterWorker(Theader head, TinfoAlmacenadoMasterWorker * infoAlmacenado, int *pack_size){

	char *bytes_serial;

	int espacioPackSize = sizeof(int);
	int espacioEnteros = sizeof(int) * 2;
	int espaciosVariables = infoAlmacenado->nombreTempReduccionLen+infoAlmacenado->nombreResultanteLen;
	int espacioAMallocar = HEAD_SIZE + espacioPackSize+espacioEnteros+espaciosVariables;

	if ((bytes_serial = malloc(espacioAMallocar)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);



	memcpy(bytes_serial + *pack_size, &infoAlmacenado->nombreTempReduccionLen, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoAlmacenado->nombreTempReduccion, infoAlmacenado->nombreTempReduccionLen);
	*pack_size += infoAlmacenado->nombreTempReduccionLen;

	memcpy(bytes_serial + *pack_size, &infoAlmacenado->nombreResultanteLen, sizeof(int));
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, infoAlmacenado->nombreResultante, infoAlmacenado->nombreResultanteLen);
	*pack_size += infoAlmacenado->nombreResultanteLen;


	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;
}


TinfoAlmacenadoMasterWorker *deserializeInfoAlmacenadoMasterWorker(char *bytes_serial){

	int off;
	TinfoAlmacenadoMasterWorker *infoAlmacenado;

		if ((infoAlmacenado = malloc(sizeof (TinfoAlmacenadoMasterWorker))) == NULL){
			log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
			return NULL;
		}

		off = 0;


		memcpy(&infoAlmacenado->nombreTempReduccionLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((infoAlmacenado->nombreTempReduccion = malloc(infoAlmacenado->nombreTempReduccionLen)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}

		memcpy(infoAlmacenado->nombreTempReduccion, bytes_serial + off, infoAlmacenado->nombreTempReduccionLen);
		off += infoAlmacenado->nombreTempReduccionLen;

		memcpy(&infoAlmacenado->nombreResultanteLen, bytes_serial + off, sizeof (int));
		off += sizeof (int);

		if ((infoAlmacenado->nombreResultante = malloc(infoAlmacenado->nombreResultanteLen)) == NULL){
			printf("No se pudieron mallocar %d bytes al Paquete De Bytes\n", infoAlmacenado->nombreResultanteLen);
			return NULL;
		}

		memcpy(infoAlmacenado->nombreResultante, bytes_serial + off, infoAlmacenado->nombreResultanteLen);
		off += infoAlmacenado->nombreResultanteLen;

		return infoAlmacenado;
}

char * serializarInfoArchivoYamaFS(Theader head,TinfoArchivoFSYama *infoArchivo,int *pack_size){

		char *bytes_serial;

		int espacioMalloc=HEAD_SIZE + sizeof(int)*2;

		int i;
		for(i=0;i<infoArchivo->listaSize;i++){
			TpackageUbicacionBloques *aux = list_get((infoArchivo->listaBloques),i);

			espacioMalloc+=aux->nombreNodoC1Len;
			espacioMalloc+=aux->nombreNodoC2Len;
			espacioMalloc+=6*sizeof(int);

		}


		if ((bytes_serial = malloc(espacioMalloc)) == NULL){
			log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
			return NULL;
		}


		*pack_size = 0;
		memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
		*pack_size += HEAD_SIZE;

		// hacemos lugar para el payload_size
		*pack_size += sizeof(int);

		memcpy(bytes_serial + *pack_size, &infoArchivo->listaSize, sizeof(int));
		*pack_size += sizeof(int);

	for(i=0;i<infoArchivo->listaSize;i++){
		TpackageUbicacionBloques *aux = list_get(infoArchivo->listaBloques,i);
		memcpy(bytes_serial + *pack_size, &aux->bloque, sizeof(int));
		*pack_size += sizeof(int);

		memcpy(bytes_serial + *pack_size, &aux->nombreNodoC1Len, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, aux->nombreNodoC1, aux->nombreNodoC1Len);
		*pack_size += aux->nombreNodoC1Len;
		memcpy(bytes_serial + *pack_size, &aux->bloqueC1, sizeof(int));
		*pack_size += sizeof(int);

		memcpy(bytes_serial + *pack_size, &aux->nombreNodoC2Len, sizeof(int));
		*pack_size += sizeof(int);
		memcpy(bytes_serial + *pack_size, aux->nombreNodoC2, aux->nombreNodoC2Len);
		*pack_size += aux->nombreNodoC2Len;
		memcpy(bytes_serial + *pack_size, &aux->bloqueC2, sizeof(int));
		*pack_size += sizeof(int);

		memcpy(bytes_serial + *pack_size, &aux->finBloque, sizeof(int));
		*pack_size += sizeof(int);

	}

	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

return bytes_serial;

}

TinfoArchivoFSYama *deserializarInfoArchivoYamaFS(char * buffer){

	int off;
	TinfoArchivoFSYama *infoArchivo;

	if ((infoArchivo = malloc(sizeof (TinfoArchivoFSYama))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	off = 0;

	memcpy(&infoArchivo->listaSize, buffer + off, sizeof (int));
	off += sizeof (int);

	infoArchivo->listaBloques=list_create();

	int i;
	t_list *listaInfoBloques = list_create();
	for(i=0;i<infoArchivo->listaSize;i++){

		TpackageUbicacionBloques *bloqueAux = malloc(sizeof (TpackageUbicacionBloques));

		memcpy(&bloqueAux->bloque, buffer + off, sizeof (int));
		off += sizeof (int);

		memcpy(&bloqueAux->nombreNodoC1Len, buffer + off, sizeof (int));
		off += sizeof (int);
		if ((bloqueAux->nombreNodoC1 = malloc(bloqueAux->nombreNodoC1Len)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(bloqueAux->nombreNodoC1, buffer + off, bloqueAux->nombreNodoC1Len);
		off += bloqueAux->nombreNodoC1Len;

		memcpy(&bloqueAux->bloqueC1, buffer + off, sizeof (int));
		off += sizeof (int);

		memcpy(&bloqueAux->nombreNodoC2Len, buffer + off, sizeof (int));
		off += sizeof (int);
		if ((bloqueAux->nombreNodoC2 = malloc(bloqueAux->nombreNodoC2Len)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(bloqueAux->nombreNodoC2, buffer + off, bloqueAux->nombreNodoC2Len);
		off += bloqueAux->nombreNodoC2Len;

		memcpy(&bloqueAux->bloqueC2, buffer + off, sizeof (int));
		off += sizeof (int);

		memcpy(&bloqueAux->finBloque, buffer + off, sizeof (int));
		off += sizeof (int);

		list_add(listaInfoBloques,bloqueAux);
	}
	infoArchivo->listaBloques=listaInfoBloques;

	return infoArchivo;
}


char * serializarInfoNodosYamaFS(Theader head,TinfoNodosFSYama *infoNodos,int *pack_size){

	char *bytes_serial;

	int espacioMalloc=HEAD_SIZE + sizeof(int)*2;

	int i;
	for(i=0;i<infoNodos->listaSize;i++){
		TpackageInfoNodo *aux = list_get((infoNodos->listaNodos),i);
		espacioMalloc+=aux->tamanioIp;
		espacioMalloc+=aux->tamanioNombre;
		espacioMalloc+=aux->tamanioPuerto;
		espacioMalloc+=3*sizeof(int);
	}


	if ((bytes_serial = malloc(espacioMalloc)) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}


	*pack_size = 0;
	memcpy(bytes_serial + *pack_size, &head, HEAD_SIZE);
	*pack_size += HEAD_SIZE;

	// hacemos lugar para el payload_size
	*pack_size += sizeof(int);

	memcpy(bytes_serial + *pack_size, &infoNodos->listaSize, sizeof(int));
	*pack_size += sizeof(int);


	for(i=0;i<infoNodos->listaSize;i++){
			TpackageInfoNodo *aux = list_get(infoNodos->listaNodos,i);

			memcpy(bytes_serial + *pack_size, &aux->tamanioNombre, sizeof(int));
			*pack_size += sizeof(int);
			memcpy(bytes_serial + *pack_size, aux->nombreNodo, aux->tamanioNombre);
			*pack_size += aux->tamanioNombre;

			memcpy(bytes_serial + *pack_size, &aux->tamanioIp, sizeof(int));
			*pack_size += sizeof(int);
			memcpy(bytes_serial + *pack_size, aux->ipNodo, aux->tamanioIp);
			*pack_size += aux->tamanioIp;


			memcpy(bytes_serial + *pack_size, &aux->tamanioPuerto, sizeof(int));
			*pack_size += sizeof(int);
			memcpy(bytes_serial + *pack_size, aux->puertoWorker, aux->tamanioPuerto);
			*pack_size += aux->tamanioPuerto;

		}

	memcpy(bytes_serial + HEAD_SIZE, pack_size, sizeof(int));

	return bytes_serial;


}

TinfoNodosFSYama * deserializarInfoNodosFSYama(char * buffer){

	int off;
	TinfoNodosFSYama *infoNodos;

	if ((infoNodos = malloc(sizeof (TinfoArchivoFSYama))) == NULL){
		log_error(logError, "No se pudo mallocar espacio para paquete de bytes.");
		return NULL;
	}

	off = 0;

	memcpy(&infoNodos->listaSize, buffer + off, sizeof (int));
	off += sizeof (int);

	infoNodos->listaNodos=list_create();

	int i;
	t_list *listaInfoNodos = list_create();
	for(i=0;i<infoNodos->listaSize;i++){

		TpackageInfoNodo *nodoAux = malloc(sizeof (TpackageInfoNodo));

		memcpy(&nodoAux->tamanioNombre, buffer + off, sizeof (int));
		off += sizeof (int);
		if ((nodoAux->nombreNodo = malloc(nodoAux->tamanioNombre)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(nodoAux->nombreNodo, buffer + off, nodoAux->tamanioNombre);
		off += nodoAux->tamanioNombre;

		memcpy(&nodoAux->tamanioIp, buffer + off, sizeof (int));
		off += sizeof (int);
		if ((nodoAux->ipNodo = malloc(nodoAux->tamanioIp)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(nodoAux->ipNodo, buffer + off, nodoAux->tamanioIp);
		off += nodoAux->tamanioIp;


		memcpy(&nodoAux->tamanioPuerto, buffer + off, sizeof (int));
		off += sizeof (int);
		if ((nodoAux->puertoWorker = malloc(nodoAux->tamanioPuerto)) == NULL){
			log_error(logError,"error malloc ");
			return NULL;
		}
		memcpy(nodoAux->puertoWorker, buffer + off, nodoAux->tamanioPuerto);
		off += nodoAux->tamanioPuerto;
		list_add(listaInfoNodos,nodoAux);

	}
	infoNodos->listaNodos=listaInfoNodos;

	return infoNodos;
}
