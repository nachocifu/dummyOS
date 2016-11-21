#include <net.h>
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

static uint8_t myMac[6];


ethframe frame;
	

static uint8_t receiveBuffer[BUF_SIZE] = {0};


/**
	Esta funcion hablar con un la configuracion del PCI y cada dispositivo.
	Sacado de OSDev.
*/
uint16_t pciConfigReadWord (uint8_t bus, uint8_t func, uint8_t offset){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = 0;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
 }
 /**
	Esta funcion hablar con un la configuracion del PCI y cada dispositivo.
	Sacado de OSDev.
*/
void pciConfigWriteWord (uint8_t bus, uint8_t func, uint8_t offset, uint8_t value){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = 0;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    sysOutLong (0xCFC, value);
}


/**
	Habilitamos DMA para nuestros perifericos.
*/
void dma_start(){
  uint32_t reg = pciConfigReadWord(0, 0x18, 0x04); //Esto es el PCI Configuration Space de la placa.
  reg |= (1<<2); //Pongo el bit 2 en 1 para habilitar DMA para que pueda escribir en la memoria (los buffers).
  pciConfigWriteWord(0, 0x18, 0x04, reg);
}

void net_start(){

	

	sysOutByte( IOADDR + 0x52, 0x0); //Aca prendemos la placa de red pasandole un 0x00 en el registro 0x52.



	sysOutByte( IOADDR + 0x37, 0x10); //Aca reseteamos la placa mandandole un 0x10 al registro 0x37.
	while( (sysInByte(IOADDR + 0x37) & 0x10) != 0) { } //Me quedo esperando hasta que el registro devuelva un 0x10.



	//Aca le pasamos a la placa de red la direccion de memoria de lo que seria mi buffer de lectura.
	sysOutLong(IOADDR + 0x30, (uint32_t)receiveBuffer); 



	//Aca le definimos la mascara de interupciones de la placa de red, pongo todo en 1.
	sysOutWord(IOADDR + 0x3C, 0x000f);



	//Aca configuramos el buffer de recepcion, le digo que accepte todo y que trate al buffer como un verctor tradicional.
	sysOutLong(IOADDR + 0x44, 0xf | (1 << 7));


	//Aca prendemos la recepcion y emicion de paquetes. Esto basicamente habilita la comunicacion.
	sysOutByte(IOADDR + 0x37, 0x0C);


	//Esto es el puntero a la memoria a transmitir, frame es basado en el protocolo de ethernet.
	//En realidad hay 4 punteros a memoria que se pueden usar pero solo usamos 1.
	sysOutLong(TSAD0, (uint32_t)&frame);


	for(int i = 0; i < MAC_SIZE ; i++){
		frame.hdr.src[i] = sysInByte(IOADDR + i); //Saco la mac del registro 0x00 y lo guardo en el frame ethernet.
		myMac[i] = sysInByte(IOADDR + i); //Guardo en memoria local.
	}
}

/**
	Aca me interumpio la placa de red. Me tengo que fijar no mas que me dice, si me llego algo o envie algo correctamente.
	Si envie correctamente, no hago nada.
	Si recibi correctamente, leo del buffer.
*/
void rtlHandler(){
	int i;
	uint16_t isr = sysInWord(ISR); //ISR es el registro de estado que quiero ver lo que hizo la placa.

	switch(isr){
		case TRANSMIT_OK:{
			//lcPrint("Transmited Something!!!!!!!");
			//lcNewLine();
			break;
		}
		case RECEIVE_OK:{
			//lcPrint("Recived Something: ");
			//int index = 0;
			//while (receiveBuffer[index + RX_DATA_OFFSET] != 0){
				//lcPrintChar((char)receiveBuffer[index + RX_DATA_OFFSET]);
				//index++;
			//}
			//lcNewLine();
			break;
		}
		default:{
			//lcPrint("Recived Interrupt with wierd data: ");
			//lcPrintChar(isr);
			//lcNewLine();
			break;
		}
	}
	
	net_start(); 
}



int net_read(char *b){
	int index = 0;
	while (receiveBuffer[index + RX_DATA_OFFSET] != 0 && 
			(isPrintable(receiveBuffer[index + RX_DATA_OFFSET]) || receiveBuffer[index + RX_DATA_OFFSET] == '\n')){
		b[index] = (char)receiveBuffer[index + RX_DATA_OFFSET];
		receiveBuffer[index + RX_DATA_OFFSET] = 0;
		index++;
	}
	b[index] = 0;
	return index;
}

void net_send(char *msg, char *mac){

	for (int i = 0; i < MAC_SIZE; i++)
		frame.hdr.dst[i] = mac[i];
	
	memcpy(frame.data, msg, mystrlen(msg)); //Data

	frame.hdr.proto = ETH_P_802_3; //Protocolo

	uint32_t size = ETH_HLEN + mystrlen(msg); //Tamano de la transmision.
	
	while (!(sysInLong(TSD0) & TSD_OWN)){ }

	sysOutLong(TSD0, size);
}


