/* sampleCodeModule.c */
#include <libc.h>
#include <manpages.h>

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
		printf("user@dummyOS $ ");
		scanf("%s", &f);
		char** params = str_split(f);
		exit = parseCommand(params);
	}


 
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}

int parseCommand(char **params) {
	
	if ( strcmp((char *)params[0], "echo") == 0 ) {
		printf("%s", (char *)params[1]);
		printf("%s", (char *)params[2]);
		printf("%s", (char *)params[3]);
		printf("%s\n", (char *)params[4]);
	} else if ( strcmp((char *)params[0], "time") == 0 ) {
		printf("Printf del tiempo del sistema\n");
	} else if ( strcmp((char *)params[0], "net-msg") == 0 ) {
		printf("Net messaging not implemented ! \n");
		printf("Message Data: \n");
		printf("    Destination Address: %s \n", (char *)params[1]);
		printf("    Message: %s \n", strconcat(strconcat(strconcat((char *)params[2], " "), strconcat((char *)params[3], " ")),(char *)params[4]));  
	} else if ( strcmp((char *)params[0], "help") == 0 ) {
		printf("DummyOS Available Commands !\n");
		printf("net-msg    help    man    time    clear\n");
	} else if ( strcmp((char *)params[0], "man") == 0 ) {
		printManPage((char *)params[1]);
	} else if ( strcmp((char *)params[0], "exit") == 0 ) {
		return TRUE;
	} else if ( strcmp((char *)params[0], "clear") == 0 ) {
		for (int i = 0; i < 78; ++i)
		 printf("\n");
	}
	
	return FALSE;

}

void printManPage (char *command) {
	if ( strcmp(command, "net-msg") == 0 ) {
		printf("%s\n", man_net_msg);
	} else if ( strcmp(command, "man") == 0 ) {
		printf("%s\n", man_man);
	} else if ( strcmp(command, "time") == 0 ) {
		printf("%s\n", man_time);
	} else if ( strcmp(command, "help") == 0 ) {
		printf("%s\n", man_help);
	} else if ( strcmp(command, "clear") == 0 ) {
		printf("%s\n", man_clear);
	} else
		printf("%s%s\n", man_null, command);
}

void initWelcome () {
	printf(
		"    *****************************************\n*                                       *\n*        Welcome To DummyOS             *\n*                                       *\n*****************************************\n"
		);
	return;
}
