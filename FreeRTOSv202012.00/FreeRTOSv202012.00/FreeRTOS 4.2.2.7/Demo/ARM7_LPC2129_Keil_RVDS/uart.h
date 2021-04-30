#define TERMINATOR 13
#define RECEIVER_SIZE 16
#define TRANSMITER_SIZE 20

enum eTransmiterStatus {FREE, BUSY};

enum eRecieverStatus { EMPTY, READY, OVERFLOW};  

void UART_InitWithInt(unsigned int uiBaudRate);
void Reciever_PutCharacterToBuffer(char cCharacter);
enum eRecieverStatus eReciever_GetStatus(void);
void Reciever_GetStringCopy(char *ucDestination);

void Transmiter_SendString(char pcString[]);
enum eTransmiterStatus Transmiter_GetStatus(void);
