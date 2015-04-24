#include "emif.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_emif.h"

#ifdef __cplusplus
}
#endif

#define EMIF_DATA	((volatile Uint16*)0x00100000)

#pragma CODE_SECTION("ramfuncs")
void emifInit()
{
	// EMIF init
	Emif1Regs.ASYNC_WCCR.bit.MAX_EXT_WAIT	= 0x80;		// Max wait clk cycles
	Emif1Regs.ASYNC_WCCR.bit.WP0 			= 0;		// Wait active low
	
	Emif1Regs.ASYNC_CS2_CR.bit.ASIZE		= 1;        // 16-bit bus
    Emif1Regs.ASYNC_CS2_CR.bit.TA			= 3;        // Turn Around cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.R_HOLD		= 7;        // Read Strobe Hold cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.R_STROBE		= 0x3F;     // Read Strobe Duration cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.R_SETUP		= 0xF;      // Read Strobe Setup cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.W_HOLD		= 7;        // Write Strobe Hold cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.W_STROBE		= 0x3F;     // Write Strobe Duration cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.W_SETUP		= 0xF;      // Write Strobe Setup cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.EW			= 0;        // Disable Extend Wait mode.
    Emif1Regs.ASYNC_CS2_CR.bit.SS			= 0;        // Disable Select Strobe mode.
	
    Emif1Regs.INT_MSK_SET.bit.WR_MASK_SET = 1;
    Emif1Regs.INT_MSK_SET.bit.LT_MASK_SET = 1;

	Emif1Regs.INT_MSK_CLR.bit.AT_MASK_CLR	= 1;        // Clear Asynchronous Timeout interrupt.
    Emif1Regs.INT_MSK_CLR.bit.LT_MASK_CLR	= 1;        // Clear Line Trap interrupt.
    Emif1Regs.INT_MSK_CLR.bit.WR_MASK_CLR	= 4;        // Clear Wait Rise interrupt.


}

#pragma CODE_SECTION("ramfuncs")
void emifWrite(Uint16 address, Uint16 data)
{
	//Uint16 *cpldEmifData = (Uint16 *)EMIF_DATA;
	//cpldEmifData[address] = data;
	EMIF_DATA[address] = data;
}

#pragma CODE_SECTION("ramfuncs")
Uint16 emifRead(Uint16 address)
{
	//Uint16 *cpldEmifData = (Uint16 *)EMIF_DATA;
	//return cpldEmifData[address];
	return EMIF_DATA[address];
}
