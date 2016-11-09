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
void scanf(){

}

void printf(char *format, ...){
	
	//Revisar que el string es correcto.



	char output[200];

	int index = 0;
	int num_args = 0;
	while(format[index] != 0){
		if (format[index] == '%'){
			num_args++;
		}
		index++;
	}
	index = 0;
	int indexOutput = 0;
	va_list ap;
	va_start(ap, num_args);
	while(format[index] != 0){
		if (format[index] == '%'){
			switch(format[++index]){
				case 'c':{
					char value = (char)va_arg(ap, int);
					output[indexOutput++] = value;
					break;
				}
				case 'd':{
					//TODO
					char number[20];
					int characters = uintToBase(va_arg(ap, int), number, 10);
					for (int j = 0; j < characters; j++){
						output[indexOutput++] = number[j];
					}
					break;
				}
				case 's':{
					char *string = va_arg(ap, char*);
					int indexAux = 0;
					while (string[indexAux] != 0){
						output[indexOutput++] = string[indexAux++];
					}
					break;
				}
				case 'f':{
					break;
				}
				case 'u':{
					break;
				}
			}
			
		}else{
			output[indexOutput++] = format[index];
		}
		index++;
	}
	output[indexOutput] = 0;
	va_end(ap);
	
	ncPrint(output);
	
	//return output;

}

