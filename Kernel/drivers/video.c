#include <stdarg.h>
#include <drivers.h>
#include <naiveConsole.h>
/**
 * Esta driver es el que se ocupa de manejo de input y output de la pantalla.
 * Tecnico: Este driver te da la linea de edicion (ultima) y si hay un scanf (por ejempo) devuelver el valor, etc, etc.
 */

void keyboardEvent(uint8_t c, int function){
	switch(function){
		case RESPONSE_CHARACTER:{
			ncPrintChar((unsigned char)c);
			break;
		}
		case RESPONSE_BACKSPACE:{
			ncPrintChar((char)'.');
			break;
		}
		case RESPONSE_ENTER:{
			ncNewline();
			break;
		}
		case RESPONSE_ARROWS:{
			ncPrintDec(c);
			break;
		}
	}
}

void backspace(){

}

void newLine(){

}
void read(){

}
void write(){
	
}


void scanf(){

}



