/* sampleCodeModule.c */
#include <libc.h>

void initWelcome ();
char * v = (char*)0xB8000;

static int var1 = 0;
static int var2 = 0;
static int exit = FALSE;
static int k = 0;

int main() {
	
	// Welcomeing sign
	initWelcome();

	// Method containing tests. 
	// To be used instead of commenting o erasing code in main.
	// Prevents git merge conflicts
	// testingMethod();
	
	// Loop shell until user exits OS
	while(!exit){
		char f[100];
		printf("yourName@dummyOS $ ");
		scanf("%s", &f);
		printf("El comando ingresado fue: %s",f);
		char** params = str_split(f);
		
		parseCommand(params);
	}


 
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}

void parseCommand(char (**params)) {
	printf("Hola Mundo!\n");
	
	printf("PARAM = %s\n", (char *)params[0]);
	printf("PARAM = %s\n", (char *)params[1]);
	printf("PARAM = %s\n", (char *)params[2]);
	// printf("PARAM = %s\n", params[3]);
	// printf("PARAM = %s\n", params[4]);

}

void initWelcome () {
	printf("Welcome To DummyOS\n\n");
	return;
}

void testingMethod() {
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
}



