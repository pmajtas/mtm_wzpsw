#include "string.h"

#define HEX_bm 0x000F 


void CopyString(char pcSource[], char pcDestination[])
{
	unsigned char ucCharCounter;

	for (ucCharCounter=0 ; pcSource[ucCharCounter] !='\0'; ucCharCounter++)
	{
		pcDestination[ucCharCounter] = pcSource[ucCharCounter];
		
	}
	pcDestination[ucCharCounter] = '\0';
}


void AppendString(char pcSourceStr[], char pcDestinationStr[])
{
	unsigned char ucCharCounter;

	for (ucCharCounter = 0; pcDestinationStr[ucCharCounter] != '\0'; ucCharCounter++)
	{
	}
		CopyString(pcSourceStr, pcDestinationStr+ucCharCounter);

}


void UIntToHexStr(unsigned int uiValue, char pcStr[])
{
	unsigned char ucNibbleCounter;
	unsigned char ucCurrentNibble;

	pcStr[0] = '0';
	pcStr[1] = 'x';

	for (ucNibbleCounter = 0; ucNibbleCounter < 4; ucNibbleCounter++)
	{
		ucCurrentNibble = ((uiValue >> ucNibbleCounter * 4) & HEX_bm);

		if (ucCurrentNibble > 9)
		{
			pcStr[5 - ucNibbleCounter] = ucCurrentNibble - 10 + 'A';
		}
		else
		{
			pcStr[5 - ucNibbleCounter] = ucCurrentNibble + '0';
		}
	
	}
	pcStr[6] = '\0';
}

void AppendUIntToString(unsigned int uiValue, char pcDestinationStr[])
{
	unsigned char ucEndPointer;
	for (ucEndPointer = 0; pcDestinationStr[ucEndPointer] != '\0'; ucEndPointer++) {}
	UIntToHexStr(uiValue, pcDestinationStr + ucEndPointer);
	}
	
