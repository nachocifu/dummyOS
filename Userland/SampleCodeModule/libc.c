#include <libc.h>

int syscall(int a, int b, int c, int d);

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}


void printf(char *format, ...){
	
	//TODO: Revisar que el string es correcto.

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
	
	syscall(0, output, indexOutput, 0);
}

int pow(int base, int exp){
	int res = 1;
	for (int i = 0; i < exp; i++){
		res = res*base;
	}
	return res;
}

int stringToInt(char *str, int lenght){
	int magnitud = pow(10, lenght-1);
	int result = 0;
	for (int i = 0; i < lenght; i++){
		result = result + ((int)(str[i] - '0'))*magnitud;
		magnitud = magnitud/10;
	}
	return result;
}

/**
 *La funcion scanf funciona como la clasica funcion de scanf.
 *La particularidad de esta en este proyecto es que para recibir el valor de pantalla hace polling.
 *La funcion le entran los characteres ni bien se ingresan en pantalla pero unicamente los devuelve cuando
 *el usuario presiono "return" (Esto se ve cuando le llega un character '\n').
 *En caso que el usuario quiera un "feed" en tiempo real de la pantalla se puede crear otra funcion de tipo scan
 *que simplemente no espere ningun character especial.
 */
void scanf(char *format, ...){

	//TODO: Revisar que el string sea valido y implementar que scan devuelva los valores en los punteros ingresados.

	char str[200]; //Este buffer es el input entero del user.
	char end = 0;
	char buffer[10]; //Buffer es un pequeño buffer de los valores que devuelve el syscall read, como lo estoy llamando adentro
					 //de un loop este buffer nunca se va a llenar mucho entonces su tamaño no es importante.
					 //Para que este buffer se llene el usuario tiene que apretar 10 teclas a mas rapido de lo que el kernel 
					 //tarde en responder el syscall.

	int indexStr = 0;

	while(!end){
		int lon = syscall(1, buffer, 0, 0); //Llamo a la syscall, me devuelve la longitud de lo devuelto en 'buffer'.
		for (int i = 0; i < lon; i++){
			str[indexStr++] = buffer[i]; //Concateno buffer al final de 'str'.
			if (buffer[i] == '\n'){
				end = 1; //Si encontre un '\n' entonces el usuario apreto "return", puedo terminar.
			}
			if (buffer[i] == '\b'){ //Si aca llega un '\b' significa que el usuario apreto backspace y en la consola, visualmente, ya se elimino el character.
				indexStr--;
				indexStr--;//Menos 2 porque en caso que haya un backspace en el buffer, tengo que volver a la original del loop y eliminar la anterior.
			}
		}
	}
	str[indexStr] = 0; //Pongo un 0 al final del string del usuario para que funcione bien en C.


	va_list ap;
	va_start(ap, 1);

	int index = 0; //Este es el index del string de formato.
	while(format[index] != 0){
		if (format[index] == '%'){ //Si el character es un '%' entonces el proximo character es el formato.
			char type = format[++index]; //Aca tomo el proximo.
			switch(type){
				case 'c':{ //Tipo char.
					char *d = va_arg(ap, char*);
					*d = str[0];
					break;
				}
				case 'd':{ //Tipo integer.
					int *d = va_arg(ap, int*);
					int a = stringToInt(str, indexStr-1); //indexStr es mas largo que el valor porque toma en cuenta el 0.
					*d = a;
					break;
				}
				case 's':{ //Tipo String.
					char *d = va_arg(ap, char*);
					for (int i = 0; i < indexStr + 1; i++) //El +1 es para que meta el 0 al final.
						d[i] = str[i];
					break;
				}
				case 'f':{ //Tipo float.
					break;
				}
				case 'u':{ //Tipo unsigned.
					break;
				}
			}

		}
		index++;
	}


}

// static void *ptr = NULL;

// mm_init() {
//   ptr = syscall(GET_FREE_MEMORY);
// };

// void *malloc(size_t size) {
// 	void* ret = ptr;
// 	ptr += size;
// 	return ret;
// }

