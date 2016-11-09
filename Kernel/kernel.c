#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>
#include <drivers.h>

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


int sysCallDispacher(int function, char* segundo, int tercero, int cuarto){

	switch(function){
		case 0:{
			write(segundo, tercero);
			break;
		}
		case 1:{
			//ncPrint("SysCall read.");
			int t = read(segundo);
			return t;
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
			write(&c, 1);
			break;
		}
		case RESPONSE_BACKSPACE:{
			//write((char)'.', 1);
			break;
		}
		case RESPONSE_ENTER:{
			newLine();
			break;
		}
		case RESPONSE_ARROWS:{
			//ncPrintDec(c);
			break;
		}
		
	}
}

int main(){

	// TESTING
	//printf("test te%sst %c te%dst %s", "EEEeeeEEee",'c', 1234567890, "hola que tal");


	// Kernel INIT
	init_interruptions();

	// Kernel Operations
	setKeyboardCallback(miCallbacldeTeclado);	

	// UserLand Init
	((EntryPoint)sampleCodeModuleAddress)();
}
