/* sampleCodeModule.c */
#include <libc.h>
#include <commands.h>

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
		printf("dummyOS $ ");
		scanf("%s", &f);
		char** params = str_split(f);
		exit = runCommand(parseCommand(params[0]), params);
	}


 
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}

void initWelcome () {
	printf(
		"    *****************************************\n*                                       *\n*        Welcome To DummyOS             *\n*                                       *\n*****************************************\n"
		);
	return;
}
