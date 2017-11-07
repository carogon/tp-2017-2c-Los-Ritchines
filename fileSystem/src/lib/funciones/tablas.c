#include "../funcionesFS.h"

void eliminarKeyDeArchivo(char * rutaArchivo, char * key){
	FILE * archivo = fopen(rutaArchivo, "r+");
	char * buffer = malloc(100);
	int contiene;
	int fd = fileno(archivo);
	unsigned long long i = 0, indiceParaArchivoFinal = 0, indicePasadorDeInfo = 0;
	unsigned long long tamanio = tamanioArchivo(archivo);
	char * archivoMapeado = mmap(NULL, tamanio, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	while(i < tamanio){
		indiceParaArchivoFinal = 0;
		while(archivoMapeado[i] != '\n'){
			buffer[indiceParaArchivoFinal] = archivoMapeado[i];
			i++;
			indiceParaArchivoFinal++;
		}
		buffer[indiceParaArchivoFinal] = '\0';
		if((int)string_contains(buffer, key)){
			break;
		}
		i++;
	}
	indiceParaArchivoFinal=0;
	i -= strlen(buffer);
	truncate("/home/utnso/holi.txt", tamanio - strlen(buffer) - 1);
	FILE * archivo2 = fopen("/home/utnso/holi.txt", "r+");
	int fd2 = fileno(archivo2);
	char * archivoFinal = mmap(NULL, tamanio - strlen(buffer) + 1 , PROT_WRITE, MAP_SHARED, fd2, 0);
	while(indicePasadorDeInfo < tamanio){
		if(indicePasadorDeInfo < i || indicePasadorDeInfo > i + strlen(buffer)){
			archivoFinal[indiceParaArchivoFinal] = archivoMapeado[indicePasadorDeInfo];
			indiceParaArchivoFinal++;
		}
		indicePasadorDeInfo++;
	}

	fclose(archivo);
	fclose(archivo2);
	free(buffer);
}

void generarArrayParaArchivoConfig(t_config * archivoConf, char * key, char * dato1, char * dato2){
	char * concatenacionLoca = string_new();
	string_append(&concatenacionLoca, "[");
	string_append(&concatenacionLoca, dato1);
	string_append(&concatenacionLoca, ",");
	string_append(&concatenacionLoca, dato2);
	string_append(&concatenacionLoca, "]");
	config_set_value(archivoConf, key, concatenacionLoca);
	free(concatenacionLoca);
}

void eliminarElementoDeArrayArchivosConfig(t_config * archivoConfig, char * key, char * nombreElemento){
	char ** elementos = config_get_array_value(archivoConfig, key);
	int i = 0;
	char * definitivo = string_new();
	char * nuevoString = string_new();
	string_append(&nuevoString, "[");
	int verificacion = 0;
	while(elementos[i] != NULL){
		if(strcmp(elementos[i], nombreElemento)){
			string_append(&nuevoString, elementos[i]);
			string_append(&nuevoString, ",");
			verificacion++;
		}
		i++;
	}

	liberarPunteroDePunterosAChar(elementos);
	free(elementos);

	if(verificacion > 0){
		definitivo = string_substring(nuevoString, 0, strlen(nuevoString)-1);
		string_append(&definitivo, "]");
		config_set_value(archivoConfig, key, definitivo);
		free(definitivo);
		free(nuevoString);
		return;
	}
	string_append(&nuevoString, "]");
	config_set_value(archivoConfig, key, nuevoString);
	free(definitivo);
	free(nuevoString);
}

int sumaDeDosNumerosInt(int valor1, int valor2){
	return valor1 + valor2;
}

long sumaDeDosNumerosLong(long valor1, long valor2){
	return valor1 + valor2;
}

int restaDeDosNumerosInt(int valor1, int valor2){
	return valor1 - valor2;
}

long restaDeDosNumerosLong(long valor1, long valor2){
	return valor1 - valor2;
}

void setearAtributoDeArchivoConfigConInts(t_config * archivoConf, char * key, int value, int(*funcion)(int valor1, int valor2)){
	int tamanio = config_get_int_value(archivoConf, key);
	tamanio = funcion(tamanio, value);
	char * tamanioString = string_itoa(tamanio);
	config_set_value(archivoConf, key, tamanioString);
	free(tamanioString);
}

//problemas con los long, tener cuidado
void setearAtributoDeArchivoConfigConLongs(t_config * archivoConf, char * key, long value, long(*funcion)(long valor1, long valor2)){
	long tamanio = config_get_long_value(archivoConf, key);
	tamanio = funcion(tamanio, value);
	char * tamanioString = string_itoa(tamanio);
	config_set_value(archivoConf, key, tamanioString);
	free(tamanioString);
}

