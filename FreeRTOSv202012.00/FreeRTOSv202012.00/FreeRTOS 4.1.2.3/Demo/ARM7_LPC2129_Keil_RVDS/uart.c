#include <LPC210X.H>
#include "uart.h"
#include "string.h"


/************ UART ************/
// U0LCR Line Control Register
#define mDIVISOR_LATCH_ACCES_BIT                   0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

// UxIER Interrupt Enable Register
#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

// UxIIR Pending Interrupt Identification Register
#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       0x00000004

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

//////////////////////////////////////////////

#define RxD_UART0 01<<2
#define TxD_UART0 01<<0
#define NULL 0

struct TransmiterBuffer{
	char cData[TRANSMITER_SIZE]; 
	enum eTransmiterStatus eStatus;
	unsigned char fLastCharacter;
	unsigned char cCharCtr;
};

struct ReceiverBuffer{ 
	char cData[RECEIVER_SIZE];  
	unsigned char ucCharCtr; 
	enum eRecieverStatus eStatus;
};

////////////// Zmienne globalne ////////////

char cOdebranyZnak;
struct TransmiterBuffer sTransmiterBuff;
struct ReceiverBuffer sReceiverBuff;

///////////////////////////////////////////



char Transmiter_GetCharacterFromBuffer(void){
	char cCurrentCharacter;
	
	cCurrentCharacter=sTransmiterBuff.cData[sTransmiterBuff.cCharCtr];
	
	if(sTransmiterBuff.fLastCharacter==1){
		sTransmiterBuff.eStatus=FREE;
		sTransmiterBuff.fLastCharacter=0;
		return NULL;
	}
	else if (cCurrentCharacter==NULL){
		sTransmiterBuff.fLastCharacter=1;
		sTransmiterBuff.cCharCtr++;
		return TERMINATOR;
	}
	else{
		sTransmiterBuff.eStatus=BUSY;
		sTransmiterBuff.cCharCtr++;
		return cCurrentCharacter;
	}
}

void Reciever_PutCharacterToBuffer(char cCharacter){
	
	if(sReceiverBuff.ucCharCtr == RECEIVER_SIZE){
		sReceiverBuff.eStatus = OVERFLOW;
	}
	else if(cCharacter==TERMINATOR){
		sReceiverBuff.cData[sReceiverBuff.ucCharCtr]=NULL;
		sReceiverBuff.eStatus = READY;
		sReceiverBuff.ucCharCtr++;
	}
	else{
		sReceiverBuff.cData[sReceiverBuff.ucCharCtr]=cCharacter;;
		sReceiverBuff.ucCharCtr++;
	}
}

enum eTransmiterStatus Transmiter_GetStatus(void){
	return sTransmiterBuff.eStatus;
}

enum eRecieverStatus eReciever_GetStatus(void){
	return sReceiverBuff.eStatus;
}

void Transmiter_SendString(char pcString[]){
	
	CopyString(pcString, sTransmiterBuff.cData);

	sTransmiterBuff.eStatus = BUSY ;
	sTransmiterBuff.fLastCharacter = 0;
	sTransmiterBuff.cCharCtr = 0;
	U0THR = Transmiter_GetCharacterFromBuffer();
	
}

void Reciever_GetStringCopy(char *ucDestination){
	
	CopyString(sReceiverBuff.cData, ucDestination);
	
	sReceiverBuff.eStatus=EMPTY;
	sReceiverBuff.ucCharCtr=0;
}

__irq void UART0_Interrupt(void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii
	 unsigned char ucCurrentBufferCharacter;

   if      ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
      cOdebranyZnak = U0RBR;
			Reciever_PutCharacterToBuffer(cOdebranyZnak);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
		  ucCurrentBufferCharacter = Transmiter_GetCharacterFromBuffer();
		 
		  if(NULL!=ucCurrentBufferCharacter){
				U0THR = ucCurrentBufferCharacter;
			}
   }

   VICVectAddr = 0; // Acknowledge Interrupt
}

//////////////////////////////////////////// 
/*void UART_InitWithInt(unsigned int uiBaudRate){

   // UART0
   PINSEL0 = PINSEL0 | RxD_UART0 | TxD_UART0;                                     // ustawic pina na odbiornik uart0
   U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // dlugosc slowa, DLAB = 1
   U0DLL   = (((15000000)/16)/uiBaudRate);                      // predkosc transmisji
   U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
   U0IER  = U0IER | mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE;               // aktywujemy przerwanie pochodzace od bufora odbiornika

   // INT
	 VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel
	 VICVectCntl2  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
   VICVectAddr2  = (unsigned long)UART0_Interrupt;             // set interrupt service routine address 

}*/

//init 300 bod
void UART_InitWithInt(unsigned int uiBaudRate){
	
	unsigned long ulDivisor, ulWantedClock;
	ulWantedClock=uiBaudRate*16;
	ulDivisor=15000000/ulWantedClock;
	// UART
	PINSEL0 = PINSEL0 | 0x55;                                     // ustawic piny uar0 odbiornik nadajnik
	U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // d³ugosc s³owa, DLAB = 1
	U0DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U0DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
	U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE ;               

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel

}
