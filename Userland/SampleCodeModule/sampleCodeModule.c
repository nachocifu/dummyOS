/* sampleCodeModule.c */
#include <libc.h>

char * v = (char*)0xB8000;

static int var1 = 0;
static int var2 = 0;

static int k = 0;

int main() {
	//All the following code may be removed 

	printf("Empezo userland\n");

	char f[10];


	int valor1 = 0, valor2 = 0;
	
	while(1){
		if (valor1 == 0){
			printf("%s", "Por favor ingrese su primer numero. Este numero sera sumado al que proximamente sera solicitado en este pequeno formulario virtual (por favorrrrrrr): ");
			scanf("%d", &valor1);
		}
		if (valor2 == 0){
			printf("%s", "Ingrese el segundo: ");
			scanf("%d", &valor2);
		}
		printf("Su suma es: %d\n", valor1+valor2);
		valor1 = 0;
		valor2 = 0;

	
	}
	
	while(1){
		k++;
		if (k > 10000000){
			k = 0;
		}
	}


 
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}