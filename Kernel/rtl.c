#include <rtl.h>
#include <leeryConsole.h>

#define BUFFER_SIZE 128
#define BUF_SIZE 64
#define PROTO_SIZE 6
#define MSG_BUF_SIZE 100
#define MAX_MSG_SIZE 512

#define IOADDR 0xC000

#define TSAD0 (IOADDR + 0x20)
#define TSAD1 (IOADDR + 0x24)
#define TSAD2 (IOADDR + 0x28)
#define TSAD3 (IOADDR + 0x2C)

#define TSD0 (IOADDR + 0x10)
#define TSD1 (IOADDR + 0x14)
#define TSD2 (IOADDR + 0x18)
#define TSD3 (IOADDR + 0x1C)

#define ISR (IOADDR + 0x3E) 


#define TX_SW_BUFFER_NUM 4
#define TSD_TOK (1 << 15)
#define TSD_OWN (1 << 13)


//Bitflags del ISR
#define TRANSMIT_OK  	(1 << 2)
#define RECEIVE_OK 		1
#define ISR_ERROR		(1<<1 | 1<<3)


#define BUF_SIZE 8*1024+16
#define MAC_SIZE 6
#define PROTO_SIZE 6
#define MSG_BUF_SIZE 100
#define MAX_MSG_SIZE 512



#define RX_HEADER_SIZE 4
#define RX_DATA_OFFSET (RX_HEADER_SIZE + ETH_HLEN) //Aca arranca la data posta en el frame ethernet 
											//(antes 4 bytes de header + 2 macs + 2 de proto)
#define USER_BYTE_OFFSET (RX_HEADER_SIZE + MAC_SIZE + MAC_SIZE - 1) //Ultimo byte de la MAC de origen


#define TRUE 1
#define FALSE 0


/*
	Este es el frame que se usa para enviar 
*/

static uint8_t myMAC[6];

static struct {
	ethframe frame;
	uint32_t size;
} transmission;

static uint8_t receiveBuffer[BUF_SIZE] = {0};

typedef struct{
	char present;
	struct{
		char broadcast;
		char user;
		char data[MAX_MSG_SIZE + 1];
	} msg;
} msg_slot;


static msg_slot message_buffer[MSG_BUF_SIZE];
static int pointer = 0;
static int current = 0;


static uint8_t currentDescriptor;



void dma_init(){
  turn_on(0,0x18);
}


void turn_on(uint8_t bus, uint8_t device) {
  uint32_t reg = os_pci_read_reg(bus,device,0x04);
  reg |= (1<<2);
  os_pci_write_reg(bus,device,0x04,reg);

}

void rtl_init(){

	// ===Turning on the RTL8139 ===
	//	Send 0x00 to the CONFIG_1 register (0x52) to set the LWAKE + LWPTN to active high.
	//	This should essentially power on the device. */

	sysOutByte( IOADDR + 0x52, 0x0); //Power on


// ==== Software Reset! =======
	// Next, we should do a software reset to clear the RX and TX buffers and set everything back to defaults.
	// Do this to eliminate the possibility of there still being garbage left in the buffers or registers on power on.
	// Sending 0x10 to the Command register (0x37) will send the RTL8139 into a software reset.
	// Once that byte is sent, the RST bit must be checked to make sure that the chip has finished the reset.
	// If the RST bit is high (1), then the reset is still in operation.

	// NB: There is a minor bug in Qemu. If you check the command register before performing a soft reset,
	// you may find the RST bit is high (1). Just ignore it and carry on with the initialization procedure.
	sysOutByte( IOADDR + 0x37, 0x10);
	while( (sysInByte(IOADDR + 0x37) & 0x10) != 0) { /* nada */ }


	//Init Receive buffer

		//For this part, we will send the chip a memory location to use as its receive buffer start location.
		//One way to do it, would be to define a buffer variable and send that variables memory location

 	//ioaddr is obtained from PCI configuration
 	//sysOutLong(IOADDR + 0x30, (uintptr_t)rx_buffer);  send uint32_t memory location to RBSTART (0x30)
 	//to the RBSTART register (0x30).
	sysOutLong(IOADDR + 0x30, (uint32_t)receiveBuffer);


 //===Set IMR + ISR=======
		// The Interrupt Mask Register (IMR) and Interrupt Service Register (ISR) are responsible
		// for firing up different IRQs. The IMR bits line up with the ISR bits to work in sync.
		// If an IMR bit is low, then the corresponding ISR bit with never fire an IRQ when the time comes for it to happen.
		// The IMR is located at 0x3C and the ISR is located at 0x3E.

		// To set the RTL8139 to accept only the Transmit OK (TOK) and Receive OK (ROK) interrupts,
		// we would have the TOK and ROK bits of the IMR high and leave the rest low.
		// That way when a TOK or ROK IRQ happens, it actually will go through and fire up an IRQ.
	sysOutWord(IOADDR + 0x3C, 0x000f); // Sets the TOK and ROK bits high


//  ====Configuring receive buffer (RCR)=====

// Before hoping to see a packet coming to you, you should tell the RTL8139 to accept packets based on various rules.
// The configuration register is RCR.

// You can enable different "matching" rules:

//     AB - Accept Broadcast: Accept broadcast packets sent to mac ff:ff:ff:ff:ff:ff
//     AM - Accept Multicast: Accept multicast packets.
//     APM - Accept Physical Match: Accept packets send to NIC's MAC address.
//     AAP - Accept All Packets. Accept all packets (run in promiscuous mode).

// Another bit, the WRAP bit, controls the handling of receive buffer wrap around.

// If WRAP is 0, the Rx buffer is treated as a traditional ring buffer:
	//if a packet is being written near the end of the buffer and the RTL8139 knows you've already handled
	//data before this (thanks to CAPR), the packet will continue at the beginning of the buffer.

// If WRAP is 1, the remainder of the packet will be written contiguously
	//(overflowing the actual receive buffer) so that it can be handled more efficiently.
	//This means the buffer must be an additional 1500 bytes (to hold the largest potentially overflowing packet).

// You can also tell the size of your RX buffer here, however if you use a 8k + 16
	//buffer as described before, writing zeroes is enough.
	//To use the WRAP=1 bit, an 8K buffer must in fact be 8k+16+1500 bytes.

	sysOutLong(IOADDR + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP


// ==== Enable Receive and Transmitter ====

// Now is the time to start up the RX and TX functions.
//  This is quite an easy piece, and should (in my opinion) only be done after all
//  of the configurations to the RTL8139's registers have been set to what is desired.
//  The RE (Receiver Enabled) and the TE (Transmitter Enabled) bits are located in the Command Register (0x37).
//  Starting up the RE and TE is pretty straight-forward, but lets go through it anyways.

// To enable the RTL8139 to accept and transmit packets, the RE and TE bits must go high.
// Once this is completed, then the card will start allowing packets in and/or out.

	sysOutByte(IOADDR + 0x37, 0x0C); // Sets the RE and TE bits high


	//Hago que todos los descriptores usen el mismo buffer
	//En realidad voy a usar uno solo
	sysOutLong(TSAD0, (uint32_t)&transmission.frame);
	//sysOutLong(TSAD1, (uint32_t)&transmission.frame);
	//sysOutLong(TSAD2, (uint32_t)&transmission.frame);
	//sysOutLong(TSAD3, (uint32_t)&transmission.frame);

	//Seteo la MAC en el header del ethernet frame que vamos a usar
	//Y lo guardo en el vector de informacion
	int i;
	for(i=0; i < MAC_SIZE ; i++){
		transmission.frame.hdr.src[i] = sysInByte(IOADDR + i);
		myMAC[i] = sysInByte(IOADDR + i);
	}


	currentDescriptor = 0;
}


static int checkMAC(uint8_t* frame){

	int is_broadcast = 1;
	for(int i = 0; i < MAC_SIZE ; i++){
		ncPrintHex(frame[i]);
		if(frame[i] != 0xff){
			is_broadcast = 0;
		}
	}

	if(is_broadcast){
		return 1;
	}

	for(int i = 0; i < MAC_SIZE ; i++){
		if(frame[i] != myMAC[i])
			return 0;
	}
	return 1;
}

void rtlHandler(){
	int i;
	uint16_t isr = sysInWord(ISR);

	if(isr & TRANSMIT_OK){ 
		//Transmit OK - No hay que hacer nada
		lcPrint("Transmited Something!!!!!!!");
		lcNewLine();
	}else if(isr & RECEIVE_OK){
		lcPrint("Recived Something: ");
		for (int j = 0; j < 13; j++){
			lcPrintChar((char)receiveBuffer[RX_DATA_OFFSET + j]);
		}
		lcNewLine();
	}else{
		lcPrint("Recived Interrupt with wierd data: ");
		lcPrintChar(isr);
		lcNewLine();
	}
	
	rtl_init(); //Reseteo el dispositivo porque si no no anda
}

int strlen2(char * s){
	int c = 0;
	while(*(s++) != 0){
		c++;
	}

	return c;
}
char *videoDebug = (char *) 0xB8000;
void rtl_send(char * msg, int dst){

	int i;

	if(dst < 0){ 
	//Broadcast
	for(i=0; i < MAC_SIZE ; i++){
		transmission.frame.hdr.dst[i] = '\xff';
	}
	} else {
		//Mensaje privado
		transmission.frame.hdr.dst[0] = '\xDE';
		transmission.frame.hdr.dst[1] = '\xAD';
		transmission.frame.hdr.dst[2] = '\xC0';
		transmission.frame.hdr.dst[3] = '\xFF';
		transmission.frame.hdr.dst[4] = '\xEE';
		transmission.frame.hdr.dst[5] = dst;
	}

	uint32_t tsd = TSD0 + (currentDescriptor * 4);
	uint32_t tsad = TSAD0 + (currentDescriptor * 4);


	transmission.frame.hdr.proto = ETH_P_802_3; //Dummy type

	memcpy(transmission.frame.data, msg, strlen2(msg));


	uint32_t descriptor = ETH_HLEN + strlen2(msg); //Bits 0-12: Size
	transmission.size = descriptor;	
	descriptor &= ~(TSD_OWN); //Apago el bit 13 TSD_OWN
	descriptor &= ~(0x3f << 16);	// 21-16 threshold en 0
	
	while (!(sysInLong(tsd) & TSD_OWN)){
		videoDebug[0] = '.';
	}

	sysOutLong(tsd, descriptor);
}