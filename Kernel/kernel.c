#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>
#include <drivers.h>
#include <ethernet.h>
#include <rtl.h>




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
			write(&c, 1); //RESPONSE_CHARACTER es cuando el usuario presiona una tecla imprimible. Llamo a write del driver de video.
			break;
		}
		case RESPONSE_BACKSPACE:{
			backspace();
			break;
		}
		case RESPONSE_ENTER:{
			//char s[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0};
			//rtl_send("Hola que tal?", 2);
			newLine(); //RESPONSE_ENTER es cuando el usuario presiona "return". Llamo a "newLine" del driver de video.
			break;
		}
		case RESPONSE_ARROWS:{
			break;
		}
	}
}

uint16_t Read(uint16_t bus, uint16_t device, uint16_t function, uint16_t registeroffset){
	uint32_t id =
        0x1 << 31 //Pone un 1 en el bit mas significativo.
        | ((bus & 0xFF) << 16) //Pone el valor "bus" en los bits mas signigicativos.
        | ((device & 0x1F) << 11) //Pone el valor "device" despues.
        | ((function & 0x07) << 8) //Despues "function".
        | (registeroffset & 0xFC); //No se.
    out_l(0xCF8, id); 
    uint32_t result = in_l(0xCFC);
    return (uint16_t)((result >> ((registeroffset & 2) * 8)) & 0xffff);
}


char *videoDeb = (char *) 0xB8000;
int main(){

	// Kernel INIT
	init_interruptions();

	// Kernel Operations
	setKeyboardCallback(miCallbacldeTeclado);	

	// UserLand Init
	((EntryPoint)sampleCodeModuleAddress)();
	return;
	dma_init();
	rtl_init();


	
	char values[] = {'-', '\\', '|', '/'};
	int val = 0;
	while(1){
		videoDeb[0] = values[val++];
		if (val == 4){val = 0;}
		int i = 0;
		while (i < 50000000){i++;}
	}
	

	return;
	for (int bus = 0; bus < 256; bus++){
		for (int device = 0; device < 32; device++){
			for (int function = 0; function < 8; function++){
				int vendor_id = Read(bus, device, function, 0x00);
			    int device_id = Read(bus, device, function, 0x02);

			    int class_id = Read(bus, device, function, 0x0b);
			    int subclass_id = Read(bus, device, function, 0x0a);
			    int interface_id = Read(bus, device, function, 0x09);

			    int revision = Read(bus, device, function, 0x08);
			    int interrupt = Read(bus, device, function, 0x3c);

			    if ((vendor_id) == 0x10EC){
			    	ncPrintHex(vendor_id);
				    ncPrintChar(',');
				    ncPrintHex(device_id);
				    ncPrintChar(',');
				    ncPrintHex(class_id);
				    ncPrintChar(',');
				    ncPrintHex(subclass_id);
				    ncPrintChar(',');
				    ncPrintHex(interface_id);
				    ncPrintChar(',');
				    ncPrintHex(revision);
				    ncPrintChar(',');
				    ncPrintHex(interrupt);
				    ncPrintChar(',');
				    ncNewline();
			    }
			}
		}
	}
	ncNewline();
	ncPrint("done.");

	

	

	return 0;

}

