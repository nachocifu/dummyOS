#include <stdarg.h>
#include <drivers.h>
#include <naiveConsole.h>
/**
 * Esta driver es el que se ocupa de manejo de input y output de la pantalla.
 * Tecnico: Este driver te da la linea de edicion (ultima) y si hay un scanf (por ejempo) devuelver el valor, etc, etc.
 */

char buffer[500];
int startBuffering = FALSE;
int bufferIndex = 0;


void clean(){

}
void backspace(){

}
void newLine(){
	char nl = '\n';
	write(&nl, 1);
	ncNewline();
}
int read(char *str){

	startBuffering = TRUE;

	int i = 0;
	for (i = 0; i < bufferIndex; i++){
		str[i] = buffer[i];
		if (buffer[i] == '\n'){
			startBuffering = FALSE;
		}
	}
	str[i] = 0;

	int aux = bufferIndex;
	bufferIndex = 0;
	return aux;
}
void write(char *str, int length){
	
	for (int i = 0; i < length; i++){
		if (startBuffering){
			buffer[bufferIndex++] = str[i];
		}
		if (str[i] != '\n')
			ncPrintChar(str[i]);
	}
	
	
}