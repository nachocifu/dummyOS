#include <manpages.h>
#include <commands.h>

/**
Convert string to command int value.

@param commandName Pointer to command string value
@return int Command int value
*/
int parseCommand(char *commandName) {
	
	if ( strcmp((char *)commandName, "echo") == 0 ) 
		return COMMAND_ECHO;	
	else if ( strcmp((char *)commandName, "time") == 0 ) 
		return COMMAND_TIME;
	else if ( strcmp((char *)commandName, "net-msg") == 0 ) 
		return COMMAND_NET_MSG;
	else if ( strcmp((char *)commandName, "help") == 0 ) 
		return COMMAND_HELP;
	else if ( strcmp((char *)commandName, "man") == 0 ) 
		return COMMAND_MAN;
	else if ( strcmp((char *)commandName, "exit") == 0 ) 
		return COMMAND_POWEROFF;
	else if ( strcmp((char *)commandName, "clear") == 0 ) 
		return COMMAND_CLEAR;
	else
		return COMMAND_NULL;
	
}

/**
Run command with the submited params

@param command Int value of command to run
@param params Array of string params for command
@return int 0 or 1 if shell should exit
*/
int runCommand(int command, char **params) {

	switch (command) {
		case COMMAND_ECHO: {
			printf("%s", (char *)params[1]);
			printf("%s", (char *)params[2]);
			printf("%s", (char *)params[3]);
			printf("%s\n", (char *)params[4]);
			break;
		}
		case COMMAND_TIME: {
			printf("Printf del tiempo del sistema\n");
			break;
		}
		case COMMAND_NET_MSG: {
			printf("Net messaging not implemented ! \n");
			printf("Message Data: \n");
			printf("    Destination Address: %s \n", (char *)params[1]);
			printf("    Message: %s \n", strconcat(strconcat(strconcat((char *)params[2], " "), strconcat((char *)params[3], " ")),(char *)params[4]));  
			break;
		}
		case COMMAND_HELP: {
			printf("DummyOS Available Commands !\n");
			printf("net-msg    help    man    time    clear\n");
			break;
		}
		case COMMAND_MAN: {
			printManPage((char *)params[1]);
			break;
		}
		case COMMAND_POWEROFF: {
			return TRUE;
			break;
		}
		case COMMAND_CLEAR: {
			for (int i = 0; i < SCREEN_SIZE; ++i)
		 		printf("\n");
			break;
		}
		case COMMAND_NULL: {
			// If invalid command, do nothing
			break;
		}
	}

	return FALSE;
}

/**
Prints man page for the selected command

@param commandString Pointer to string value of command to run
*/
void printManPage (char *commandString) {
	int command = parseCommand (commandString);

	switch (command) {
		case COMMAND_ECHO: {
			printf("%s\n", man_echo);
			break;
		}
		case COMMAND_TIME: {
			printf("%s\n", man_time);
			break;
		}
		case COMMAND_NET_MSG: {
			printf("%s\n", man_net_msg);
			break;
		}
		case COMMAND_HELP: {
			printf("%s\n", man_help);
			break;
		}
		case COMMAND_MAN: {
			printf("%s\n", man_man);
			break;
		}
		case COMMAND_POWEROFF: {
			printf("%s\n", man_poweroff);
			break;
		}
		case COMMAND_CLEAR: {
			printf("%s\n", man_clear);
			break;
		}
		case COMMAND_NULL: {
			if (strlength(commandString) == 0)
				printf("%s\n", man_null);
			else
				printf("%s%s\n", man_invalid, commandString);
			
			break;
		}
	}
		
}