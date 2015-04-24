#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_spi.h"
#include <string.h>

#ifdef __cplusplus
}
#endif

void spiWrite(Uint16 numBytes, Uint8 *data);
SPI_STATUS spiRead(Uint16 numBytes, Uint8 *data_in);

#pragma CODE_SECTION("ramfuncs")
void spiInit()
{
	SpiaRegs.SPIFFTX.bit.SPIRST 		= 1;	// FIFO not reset
	SpiaRegs.SPIFFTX.bit.SPIFFENA 		= 1;	// FIFO enable
	SpiaRegs.SPIFFTX.bit.TXFIFO 		= 1;	// Transmit FIFO not reset
	SpiaRegs.SPIFFTX.bit.TXFFIENA 		= 0;	// TXFIFO interrupt disable
	SpiaRegs.SPIFFTX.bit.TXFFINTCLR 	= 1;	// Clear TXFIFO interrupt

    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR 	= 1;	// RXFIFO overflow interrupt clear
	SpiaRegs.SPIFFRX.bit.RXFIFORESET 	= 1;	// RXFIFO not reset
	SpiaRegs.SPIFFRX.bit.RXFFIENA 		= 0;	// RXFIFO interrupt disable
	SpiaRegs.SPIFFRX.bit.RXFFINTCLR 	= 1;	// RXFIFO interrupt clear

    SpiaRegs.SPIFFCT.bit.TXDLY 			= 0;	// FIFO transmit zero delay

	// SPI init
	SpiaRegs.SPICCR.bit.SPISWRESET 		= 0;	// Reset on
	SpiaRegs.SPICCR.bit.CLKPOLARITY 	= 0;	// Data out on rising edge
	//SpiaRegs.SPICCR.bit.HS_MODE 		= 0;	// Normal mode
	SpiaRegs.SPICCR.bit.SPILBK 			= 0;	// Loopback off
	SpiaRegs.SPICCR.bit.SPICHAR 		= 7;	// 8-bit character
	
	SpiaRegs.SPICTL.bit.OVERRUNINTENA	= 0;    // Overrun interrupt disable
	SpiaRegs.SPICTL.bit.CLK_PHASE 		= 0;    // No Delay
	SpiaRegs.SPICTL.bit.MASTER_SLAVE 	= 1;    // Master mode
	SpiaRegs.SPICTL.bit.TALK 			= 1;    // Transmit enable
	SpiaRegs.SPICTL.bit.SPIINTENA 		= 0;    // Interrupt disable

	SpiaRegs.SPIBRR.bit.SPI_BIT_RATE 	= 49;//4;	// SPI CLK = 50MHz/(4+1) = 10MHz
	
    SpiaRegs.SPICCR.bit.SPISWRESET 		= 1;	// Relinquish SPI from Reset  
	
	SpiaRegs.SPIPRI.bit.PRIORITY 		= 0;    // Priority
	SpiaRegs.SPIPRI.bit.FREE 			= 1;    // Set so breakpoints don't disturb xmission	
	SpiaRegs.SPIPRI.bit.STEINV 			= 0;    // SPISTE active low
	SpiaRegs.SPIPRI.bit.TRIWIRE 		= 0;    // Normal 4-wire SPI mode
}

#pragma CODE_SECTION("ramfuncs")
SPI_STATUS spiXfer(Uint16 numBytes, Uint8 *data_out, Uint8 *data_in)
{
	if (numBytes > SPI_FIFO_LENGTH)
		return SPI_FIFO_OVERFLOW;
	
	Uint8 xmtData[SPI_FIFO_LENGTH];
	Uint8 rcvData[SPI_FIFO_LENGTH];
	if (data_out)
		memcpy(xmtData, data_out, numBytes);
	else
	{
		memset(xmtData, 0, SPI_FIFO_LENGTH);
		//xmtData[0] = 0x15;
		//xmtData[1] = 0xB6;
	}
	spiWrite(numBytes, xmtData);
	
	int i;
	for (i = 0; i < 50; i++)
		asm(" NOP ");
	
	SPI_STATUS retCode = SPI_NO_ERROR;
	
	if (data_in)
		retCode = spiRead(numBytes, data_in);
	else
		retCode = spiRead(numBytes, rcvData);
		
	return retCode;
}

#pragma CODE_SECTION("ramfuncs")
void spiWrite(Uint16 numBytes, Uint8 *data)
{
	Uint16 i;
	for (i = 0; i < numBytes; i++)
		SpiaRegs.SPITXBUF = (data[i] << 8) & 0xFF00;
}

#pragma CODE_SECTION("ramfuncs")
SPI_STATUS spiRead(Uint16 numBytes, Uint8 *data_in)
{
	Uint8 rcvData[SPI_FIFO_LENGTH];
	SPI_STATUS retCode = SPI_NO_ERROR;
	
	// Wait until data is received or timeout
	Uint32 timeout = 20000; 	// 2000000
	Uint32 i = 0;
	for (i = 0; i < timeout; i++)
	{
		if (SpiaRegs.SPIFFRX.bit.RXFFST == numBytes)
			break;
	}
	
	if (i >= timeout)
		retCode = SPI_RX_TIMEOUT;
	 
	Uint16 rcvBytes = SpiaRegs.SPIFFRX.bit.RXFFST;
	
    for (i = 0; i < rcvBytes; i++)
		rcvData[i] = SpiaRegs.SPIRXBUF & 0xFF;
		
	memcpy(data_in, rcvData, numBytes);
		
	return retCode;
}
