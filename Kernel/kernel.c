#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>
#include <drivers.h>
#include <ethernet.h>
#include <net.h>
#include <syscalls.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

/**
Dispatch syscalls to the corresponding functions

@param	function Syscall to call
@param	segundo Pointer to string
@param	tercero	Length
@param	cuarto Decryptor to use
*/
int sysCallDispacher(int function, char* segundo, int tercero, int cuarto){

	switch(function){
		case SYSCALL_WRITE:{
			switch ( cuarto ) {
				case DESCRIPTOR_CLI: {
					write(segundo, tercero);
					break;
				}
				case DESCRIPTOR_NET: {
					break;
				}
				default: {
					ncPrint("SysCall not found.");
					break;
				}
			}
			break;
		}
		case SYSCALL_READ:{
			switch ( cuarto ) {
				case DESCRIPTOR_CLI: {
					return read(segundo);
					break;
				}
				case DESCRIPTOR_NET: {
					break;
				}
				default: {
					ncPrint("SysCall not found.");
					break;
				}
			}
			break;
		}
		default:{
			ncPrint("SysCall not found.");
			break;
		}
	}
	return 0;
	
}



void miCallbacldeTeclado(uint8_t c, int function){

	switch(function){
		case RESPONSE_CHARACTER:{
			write(&c, 1); //RESPONSE_CHARACTER es cuando el usuario presiona una tecla imprimible. Llamo a write del driver de video.
			break;
		}
		case RESPONSE_BACKSPACE:{
			//backspace();
			char messageBuff[100];
			int size = get_message(messageBuff);
			if (size > 0){
				for (int i = 0; i < size; i++){
					lcPrintChar((char)messageBuff[i]);
				}
				lcNewLine();
			}
			break;
		}
		case RESPONSE_ENTER:{
			char mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			net_send("Hola que tal?", mac);
			//newLine(); //RESPONSE_ENTER es cuando el usuario presiona "return". Llamo a "newLine" del driver de video.
			// rtl_send("Hola que tal?", -1);
			//newLine(); //RESPONSE_ENTER es cuando el usuario presiona "return". Llamo a "newLine" del driver de video.
			break;
		}
		case RESPONSE_ARROWS:{
			break;
		}
	}
}


char *videoDeb = (char *) 0xB8000;
int main(){

	// Kernel INIT
	init_interruptions();

	// Kernel Operations
	setKeyboardCallback(miCallbacldeTeclado);	

	
	dma_start();
	net_start();

	// UserLand Init
	//((EntryPoint)sampleCodeModuleAddress)();
	//return;
	
	


	
	
	char values[] = {'-', '\\', '|', '/'};
	int val = 0;
	while(1){
		videoDeb[0] = values[val++];
		if (val == 4){val = 0;}
		int i = 0;
		while (i < 50000000){i++;}
	}
	

	return 0;

}

