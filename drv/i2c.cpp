#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_i2c.h"
#include <stddef.h>

#ifdef __cplusplus
}
#endif

#pragma CODE_SECTION("ramfuncs")
void i2cDelay(Uint32 ticks)
{
	// 1 tick =~ 50 ns
	Uint32 i;
	for (i = 0; i < ticks; i++)
		asm(" NOP ");
}

#pragma CODE_SECTION("ramfuncs")
void i2cInit()
{
	I2caRegs.I2CMDR.bit.IRS = 0;			// Reset I2C
	
	I2caRegs.I2COAR.bit.OAR = 0x028;		// Own address
	
	I2caRegs.I2CIER.all = 0x0000;			// Disable all interrupts
	
	I2caRegs.I2CCLKL = 10;					// Low time divider. Tlow=100ns*(10+5) = 1500ns
	
	I2caRegs.I2CCLKH = 10;					// High time divider. Thigh=100ns*(10+5) = 1500ns
	
	I2caRegs.I2CSAR.bit.SAR = 0x048;		// Slave address - temperature sensor

	I2caRegs.I2CPSC.bit.IPSC = 19;	    	// Prescaler. Module clock = 10MHz

	I2caRegs.I2CMDR.bit.NACKMOD = 0;		// Always ACK
	I2caRegs.I2CMDR.bit.FREE = 1;			// Breakpoints don't disturb xmission
	I2caRegs.I2CMDR.bit.STT = 0;			// Start condition bit
	I2caRegs.I2CMDR.bit.STP = 0; 			// Stop condition bit
	I2caRegs.I2CMDR.bit.MST = 0;			// Slave mode. Master set every transfer
	I2caRegs.I2CMDR.bit.TRX = 0;			// Receiver mode. Transmitter mode set when transmit
	I2caRegs.I2CMDR.bit.XA = 0;				// 7-bit addressing mode
	I2caRegs.I2CMDR.bit.RM = 0;				// Nonrepeat mode
	I2caRegs.I2CMDR.bit.DLB = 0;			// Loopback disabled
	I2caRegs.I2CMDR.bit.IRS = 1;			// Out of reset
	I2caRegs.I2CMDR.bit.STB = 0;			// Not in start byte mode
	I2caRegs.I2CMDR.bit.FDF = 0;			// Free data format mode is disabled
	I2caRegs.I2CMDR.bit.BC = 0;				// 8-bit transfer

	I2caRegs.I2CFFTX.bit.I2CFFEN = 1;		// Enable FIFO
	I2caRegs.I2CFFTX.bit.TXFFRST = 1;		// Enable transmit FIFO
	I2caRegs.I2CFFTX.bit.TXFFINTCLR = 1;	// Clear interrupt
	I2caRegs.I2CFFTX.bit.TXFFIENA = 0;		// Disable interrupt
	I2caRegs.I2CFFTX.bit.TXFFIL = 0;		// Interrupt level
   
	I2caRegs.I2CFFRX.bit.RXFFRST = 1;		// Enable receive FIFO
	I2caRegs.I2CFFRX.bit.RXFFINTCLR = 1;	// Clear interrupt
	I2caRegs.I2CFFRX.bit.RXFFIENA = 0;		// Disable interrupt
	I2caRegs.I2CFFRX.bit.RXFFIL = 0;		// Interrupt level
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS checkNoStop()
{
	int i = 0;
	// Wait 500ns
	for (i = 0; i < 10; i++)
	{
		if (I2caRegs.I2CMDR.bit.STP == 0)
			break;
		i2cDelay(1);
	}
			
	if (I2caRegs.I2CMDR.bit.STP == 1)
		return I2C_ERR_STOP_BIT;
		
	return I2C_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS checkBusBusy()
{
	int i = 0;
	// Wait 1us
	for (i = 0; i < 10; i++)
	{
		if (I2caRegs.I2CSTR.bit.BB == 0)
			break;
		i2cDelay(2);
	}
			
	if (I2caRegs.I2CSTR.bit.BB == 1)
	{
		I2caRegs.I2CMDR.bit.MST = 0;	// Slave mode
		I2caRegs.I2CMDR.bit.TRX = 0;	// Receiver mode
		I2caRegs.I2CSTR.bit.BB = 1;		// Clear BB bit
		I2caRegs.I2CSTR.bit.NACK = 1;	// Clear NACK bit
		return I2C_ERR_BUS_BUSY;
	}
		
	return I2C_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS waitArdy()
{
	int i = 0;
	// Wait 50us
	for (i = 0; i < 250; i++)
	{
		if (I2caRegs.I2CSTR.bit.ARDY == 1)
			break;
		i2cDelay(4);
	}
			
	if (I2caRegs.I2CSTR.bit.ARDY == 0)
	{
		I2caRegs.I2CMDR.bit.MST = 0;	// Slave mode
		I2caRegs.I2CMDR.bit.TRX = 0;	// Receiver mode
		I2caRegs.I2CSTR.bit.BB = 1;		// Clear BB bit
		I2caRegs.I2CSTR.bit.NACK = 1;	// Clear NACK bit
		return I2C_ERR_ARDY;
	}
	
	I2caRegs.I2CSTR.bit.ARDY = 1;	// Clear ARDY bit
		
	return I2C_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS waitStopCondition()
{
	int i = 0;
	// Wait 100us
	for (i = 0; i < 250; i++)
	{
		if (I2caRegs.I2CSTR.bit.SCD == 1)
			break;
		i2cDelay(8);
	}
			
	if (I2caRegs.I2CSTR.bit.SCD == 0)
	{
		I2caRegs.I2CMDR.bit.MST = 0;	// Slave mode
		I2caRegs.I2CMDR.bit.TRX = 0;	// Receiver mode
		I2caRegs.I2CSTR.bit.BB = 1;		// Clear BB bit
		I2caRegs.I2CSTR.bit.NACK = 1;	// Clear NACK bit
		return I2C_ERR_SCD;
	}
	
	I2caRegs.I2CSTR.bit.SCD = 1;	// Clear SCD bit
		
	return I2C_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS i2cWrite(Uint16 address, Uint16 size, Uint8 *data, Uint8 stopCondBit)
{
	I2C_STATUS retCode;
	retCode = checkNoStop();
	if (retCode)
		return retCode;
		
	I2caRegs.I2CSAR.all = address;
	
	retCode = checkBusBusy();
	if (retCode)
		return retCode;
	
	I2caRegs.I2CCNT = size + 1;		// 1 byte address + data bytes
	
	I2caRegs.I2CDXR.all = address;
	int i;
    for (i = 0; i < size; i++)
    {
		I2caRegs.I2CDXR.all = data[i] & 0x00FF;
	}
	
	if (stopCondBit)
	{
		// Send start as master transmitter with start and stop bits
		I2caRegs.I2CMDR.all = 0x6E20;
	} else
	{
		// Send start as master transmitter with start bit and without stop bit
		I2caRegs.I2CMDR.all = 0x6620;
	}

	//wait SCD (not necessary)
	return I2C_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
I2C_STATUS i2cRead(Uint16 address, Uint16 size, Uint8 *data)
{
	I2C_STATUS retCode;
	retCode = i2cWrite(address, 0, NULL, 0);
	if (retCode)
		return retCode;
		
	i2cDelay(1000);
	
	retCode = waitArdy();
	if (retCode)
		return retCode;

	I2caRegs.I2CCNT = size;
	
	// Send restart as master receiver with start and stop bits
	//I2caRegs.I2CMDR.all = 0x6C20;
	// Start as master receiver without start bit and with stop bit
	I2caRegs.I2CMDR.all = 0x4C20;

	/*wait ARDY

	if STR.NACK == 1

	MDR.STP = 1
	STR = 0x0002*/
	
	i2cDelay(2000);

	retCode = waitStopCondition();
	if (retCode)
		return retCode;

	int i;
	for (i = 0; i < size; i++)
		data[i] = I2caRegs.I2CDRR.all & 0xFF;
	
	Uint8 dummyBuf;
	if (I2caRegs.I2CFFRX.bit.RXFFST != 0)
	{
		for (i = 0; i < I2caRegs.I2CFFRX.bit.RXFFST; i++)
			dummyBuf = I2caRegs.I2CDRR.all & 0xFF;
	}
	
	return I2C_ERR_NO_ERROR;
}
