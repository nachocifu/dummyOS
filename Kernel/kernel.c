#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>

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
char *video = (char *) 0xB8000;

void tickHandler() {
	video[i++] = i;	
}

void sti();
void irq0Handler();
void setPicMaster(uint16_t);

typedef void (*handler_t)(void);

handler_t handlers[] = {tickHandler};

void irqDispatcher(int irq) {
	handlers[irq]();
}

int main()
{	
	iSetHandler(0x20, (uint64_t) irq0Handler); 
	//todas las interrupciones se guardan en la IDT. Es una tabla. 
	//Aca le estamos diciendo que en la posicion 20 guarde la llamada a nuestro metodo a ejecutar al interrupir
	
	setPicMaster(0xFE); //esto le dice al PIC que solo escuche interrupciones de PIT (clock) no de teclado ni nadie mas
	
	sti();

	while (1) {
		int k = 0;
		while(k < 1000*1000*20) {
			k++;
		}
		ncPrintHex(i);
	}
}
