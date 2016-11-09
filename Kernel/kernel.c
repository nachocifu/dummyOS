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

static int i = 0;
static int j = 0;
static int k = 0;

static int l = 0;



char *video = (char *) 0xB8000;

void tickHandler() {
	video[i++] = i;	
}






void sti();
void irq0Handler();
void irq1Handler();
void sysCallHandler();
void setPicMaster(uint16_t);

typedef void (*handler_t)(void);

handler_t handlers[] = {tickHandler, keyboardHandler};

void irqDispatcher(int irq) {
	handlers[irq]();
}
void sysCallDispacher(int function, char* segundo, int tercero, int cuarto){


	switch(function){
		case 0:{
			ncPrint(segundo);
			ncNewline();
			break;
		}
		default:{
			ncPrint("SysCall not found.");
			break;
		}
	}
	
}

void miCallbacldeTeclado(uint8_t c, int function){
	switch(function){
		case 0:{
			ncPrintChar((unsigned char)c);
			break;
		}
		case 1:{
			ncPrintChar((char)'.');
			break;
		}
		case 2:{
			ncNewline();
			break;
		}
		case 3:{
			ncPrintDec(c);
			break;
		}
	}
	
}

int main()
{	

	setKeyboardCallback(miCallbacldeTeclado);
	//TODO: 0x21 es teclado, 0x80 syscall, 0x2C es mouse

	iSetHandler(0x20, (uint64_t) irq0Handler); 
	iSetHandler(0x21, (uint64_t) irq1Handler); //Keyboard...
	iSetHandler(0x80, (uint64_t) sysCallHandler); //Syscalls...
	//todas las interrupciones se guardan en la IDT. Es una tabla. 
	//Aca le estamos diciendo que en la posicion 20 guarde la llamada a nuestro metodo a ejecutar al interrupir
	
	setPicMaster(0xFD); //esto le dice al PIC que solo escuche interrupciones de PIT (clock) no de teclado ni nadie mas
	
	sti();

	((EntryPoint)sampleCodeModuleAddress)();
}
