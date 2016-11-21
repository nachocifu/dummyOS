/* sampleCodeModule.c */
#include <libc.h>
#include <commands.h>

void initWelcome ();
static int exit = FALSE;

int main() {
	
	// OS welcome
	initWelcome();
	
	// Loop shell until user exits OS
	while(!exit){
		char f[100];
		printf("dummyOS $ ");
		scanf("%s", &f);
		char** params = str_split(f);
		exit = runCommand(parseCommand(params[0]), params);
	}

	return 0;
}

/**
Welcoming sign of the OS
*/
void initWelcome () {
	printf(
		"    *****************************************\n*                                       *\n*        Welcome To DummyOS             *\n*                                       *\n*****************************************\n"
		);
	return;
}
