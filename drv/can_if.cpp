/*
 * can_if.cpp
 *
 *  Created on: 24 ���. 2015 �.
 *      Author: ����
 */

#include "can_if.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"
#include "F2837xD_sysctrl.h"

#ifdef __cplusplus
}
#endif

#define  EALLOW __asm(" EALLOW")
#define  EDIS   __asm(" EDIS")

//*****************************************************************************
// This is the maximum number that can be stored as an 11bit Message
// identifier.
//*****************************************************************************
#define CAN_MAX_11BIT_MSG_ID    (0x7ff)

//*****************************************************************************
// This is used as the loop delay for accessing the CAN controller registers.
//*****************************************************************************

// The maximum CAN bit timing divisor is 13.
#define CAN_MAX_BIT_DIVISOR     (13)

// The minimum CAN bit timing divisor is 5.
#define CAN_MIN_BIT_DIVISOR     (5)

// The maximum CAN pre-divisor is 1024.
#define CAN_MAX_PRE_DIVISOR     (1024)

// The minimum CAN pre-divisor is 1.
#define CAN_MIN_PRE_DIVISOR     (1)

//*****************************************************************************
// This table is used by the CANBitRateSet() API as the register defaults for
// the bit timing values.
//*****************************************************************************

static const uint16_t g_ui16CANBitValues[] =
{
    0x1100, // TSEG2 2, TSEG1 2, SJW 1, Divide 5
    0x1200, // TSEG2 2, TSEG1 3, SJW 1, Divide 6
    0x2240, // TSEG2 3, TSEG1 3, SJW 2, Divide 7
    0x2340, // TSEG2 3, TSEG1 4, SJW 2, Divide 8
    0x3340, // TSEG2 4, TSEG1 4, SJW 2, Divide 9
    0x3440, // TSEG2 4, TSEG1 5, SJW 2, Divide 10
    0x3540, // TSEG2 4, TSEG1 6, SJW 2, Divide 11
    0x3640, // TSEG2 4, TSEG1 7, SJW 2, Divide 12
    0x3740  // TSEG2 4, TSEG1 8, SJW 2, Divide 13
};

//*****************************************************************************
//! \internal
//! Checks a CAN base address.
//!
//! \param ui32Base is the base address of the CAN controller.
//!
//! This function determines if a CAN controller base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static bool
CANBaseValid(uint32_t ui32Base)
{
    return((ui32Base == CANA_BASE) || (ui32Base == CANB_BASE));
}

#endif

//*****************************************************************************
//! Initializes the CAN controller after reset.
//!
//! \param ui32Base is the base address of the CAN controller.
//!
//! After reset, the CAN controller is left in the disabled state.  However,
//! the memory used for message objects contains undefined values and must be
//! cleared prior to enabling the CAN controller the first time.  This prevents
//! unwanted transmission or reception of data before the message objects are
//! configured.  This function must be called before enabling the controller
//! the first time.
//!
//! \return None.
//
//*****************************************************************************
//#pragma CODE_SECTION("ramfuncs")
void
CANInit(uint32_t ui32Base)
{
    int16_t iMsg;

    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));

    // Place CAN controller in init state, regardless of previous state.  This
    // will put controller in idle, and allow the message object RAM to be
    // programmed.

    HWREG(ui32Base + CAN_O_CTL) = CAN_CTL_INIT;
    HWREG(ui32Base + CAN_O_CTL) =  CAN_CTL_SWR;

    // Wait for busy bit to clear
    while(HWREG(ui32Base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
    {
    }

    // Clear the message value bit in the arbitration register.  This indicates
    // the message is not valid and is a "safe" condition to leave the message
    // object.  The same arb reg is used to program all the message objects.
    HWREGH(ui32Base + CAN_O_IF1CMD + 2) = (CAN_IF1CMD_DIR | CAN_IF1CMD_ARB |
                                           CAN_IF1CMD_CONTROL) >> 16;
    HWREG(ui32Base + CAN_O_IF1ARB) = 0;
    HWREG(ui32Base + CAN_O_IF1MCTL) = 0;

    HWREGH(ui32Base + CAN_O_IF2CMD + 2) = (CAN_IF2CMD_DIR | CAN_IF2CMD_ARB |
                                           CAN_IF2CMD_CONTROL) >> 16;
    HWREG(ui32Base + CAN_O_IF2ARB) = 0;
    HWREG(ui32Base + CAN_O_IF2MCTL) = 0;

    // Loop through to program all 32 message objects
    for(iMsg = 1; iMsg <= 32; iMsg+=2)
    {
        // Wait for busy bit to clear
        while(HWREG(ui32Base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ui32Base + CAN_O_IF1CMD) = iMsg;

        // Wait for busy bit to clear
        while(HWREG(ui32Base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ui32Base + CAN_O_IF2CMD) = iMsg+1;
    }

    // Make sure that the interrupt and new data flags are updated for the
    // message objects.
    HWREGH(ui32Base + CAN_O_IF1CMD + 2) = (CAN_IF1CMD_TXRQST |
                                           CAN_IF1CMD_CLRINTPND) >> 16;
    HWREGH(ui32Base + CAN_O_IF2CMD + 2) = (CAN_IF2CMD_TXRQST |
                                           CAN_IF2CMD_CLRINTPND) >> 16;

    // Loop through to program all 32 message objects
    for(iMsg = 1; iMsg <= 32; iMsg+=2)
    {
        // Wait for busy bit to clear.
        while(HWREG(ui32Base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ui32Base + CAN_O_IF1CMD) = iMsg;

        // Wait for busy bit to clear.
        while(HWREG(ui32Base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ui32Base + CAN_O_IF2CMD) = iMsg+1;

    }

    // Acknowledge any pending status interrupts.
    HWREG(ui32Base + CAN_O_ES);

}

//*****************************************************************************
//! Select CAN peripheral clock source
//!
//! \param ui32Base is the base address of the CAN controller to disable.
//! \param ui16Source is the clock source to select for the given CAN
//!        peripheral: \n
//!        0 - Selected CPU SYSCLKOUT (CPU1.Sysclk or CPU2.Sysclk) (default at reset) \n
//!        1 - External Oscillator (OSC) clock (direct from X1/X2) \n
//!        2 - AUXCLKIN = GPIOn(GPIO19)
//!
//! Selects the desired clock source for use with a given CAN peripheral.
//!
//! \return None.
//
//*****************************************************************************
//#pragma CODE_SECTION("ramfuncs")
void CANClkSourceSelect(uint32_t ui32Base, uint16_t ui16Source)
{
    EALLOW;
    switch(ui32Base)
    {
        case CANA_BASE:
        {
            ClkCfgRegs.CLKSRCCTL2.bit.CANABCLKSEL = ui16Source;
        }

        case CANB_BASE:
        {
            ClkCfgRegs.CLKSRCCTL2.bit.CANBBCLKSEL = ui16Source;
        }

        default:
            break;
    }
    EDIS;
}

//*****************************************************************************
//! This function is used to set the CAN bit timing values to a nominal setting
//! based on a desired bit rate.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32SourceClock is the clock frequency for the CAN peripheral in Hz.
//! \param ui32BitRate is the desired bit rate.
//!
//! This function will set the CAN bit timing for the bit rate passed in the
//! \e ui32BitRate parameter based on the \e ui32SourceClock parameter.  The CAN
//! bit clock is calculated to be an average timing value that should work for
//! most systems.  If tighter timing requirements are needed, then the
//! CANBitTimingSet() function is available for full customization of all of
//! the CAN bit timing values.  Since not all bit rates can be matched
//! exactly, the bit rate is set to the value closest to the desired bit rate
//! without being higher than the \e ui32BitRate value.
//!
//! \return This function returns the bit rate that the CAN controller was
//! configured to use or it returns 0 to indicate that the bit rate was not
//! changed because the requested bit rate was not valid.
//
//*****************************************************************************
//#pragma CODE_SECTION("ramfuncs")
uint32_t
CANBitRateSet(uint32_t ui32Base, uint32_t ui32SourceClock, uint32_t ui32BitRate)
{
    uint32_t ui32DesiredRatio;
    uint32_t ui32CANBits;
    uint32_t ui32PreDivide;
    uint32_t ui32RegValue;
    uint16_t ui16CANCTL;

    ASSERT(ui32BitRate != 0);

    // Calculate the desired clock rate.
    ui32DesiredRatio = ui32SourceClock / ui32BitRate;

    // If the ratio of CAN bit rate to processor clock is too small or too
    // large then return 0 indicating that no bit rate was set.
    ASSERT(ui32DesiredRatio <= (CAN_MAX_PRE_DIVISOR * CAN_MAX_BIT_DIVISOR));
    ASSERT(ui32DesiredRatio >= (CAN_MIN_PRE_DIVISOR * CAN_MIN_BIT_DIVISOR));

    // Make sure that the Desired Ratio is not too large.  This enforces the
    // requirement that the bit rate is larger than requested.
    if((ui32SourceClock / ui32DesiredRatio) > ui32BitRate)
    {
        ui32DesiredRatio += 1;
    }

    // Check all possible values to find a matching value.
    while(ui32DesiredRatio <= CAN_MAX_PRE_DIVISOR * CAN_MAX_BIT_DIVISOR)
    {
        // Loop through all possible CAN bit divisors.
        for(ui32CANBits = CAN_MAX_BIT_DIVISOR;
            ui32CANBits >= CAN_MIN_BIT_DIVISOR;
            ui32CANBits--)
        {
            // For a given CAN bit divisor save the pre divisor.
            ui32PreDivide = ui32DesiredRatio / ui32CANBits;

            // If the calculated divisors match the desired clock ratio then
            // return these bit rate and set the CAN bit timing.
            if((ui32PreDivide * ui32CANBits) == ui32DesiredRatio)
            {
                // Start building the bit timing value by adding the bit timing
                // in time quanta.
                ui32RegValue =
                    g_ui16CANBitValues[ui32CANBits - CAN_MIN_BIT_DIVISOR];

                // To set the bit timing register, the controller must be
                // placed
                // in init mode (if not already), and also configuration change
                // bit enabled.  The state of the register should be saved
                // so it can be restored.

                ui16CANCTL = HWREG(ui32Base + CAN_O_CTL);
                HWREG(ui32Base + CAN_O_CTL) = ui16CANCTL | CAN_CTL_INIT |
                                              CAN_CTL_CCE;

                // Now add in the pre-scalar on the bit rate.
                ui32RegValue |=
                    ((ui32PreDivide - 1) & CAN_BTR_BPR_M) |
                    (((ui32PreDivide - 1) << 10) & CAN_BTR_BRPE_M) ;

                // Set the clock bits in the and the bits of the
                // pre-scalar.
                HWREG(ui32Base + CAN_O_BTR) = ui32RegValue;

                // Restore the saved CAN Control register.
                HWREG(ui32Base + CAN_O_CTL) = ui16CANCTL;

                // Return the computed bit rate.
                return(ui32SourceClock / ( ui32PreDivide * ui32CANBits));
            }
        }

        // Move the divisor up one and look again.  Only in rare cases are
        // more than 2 loops required to find the value.
        ui32DesiredRatio++;
    }
    return(0);
}

//*****************************************************************************
//! Enables individual CAN controller interrupt sources.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables specific interrupt sources of the CAN controller.  Only enabled
//! sources will cause a processor interrupt.
//!
//! The \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! - \b CAN_INT_ERROR - a controller error condition has occurred
//! - \b CAN_INT_STATUS - a message transfer has completed, or a bus error has
//! been detected
//! - \b CAN_INT_IE0 - allow CAN controller to generate interrupts on interrupt
//! line 0
//! - \b CAN_INT_IE1 - allow CAN controller to generate interrupts on interrupt
//! line 1
//!
//! In order to generate status or error interrupts, \b CAN_INT_IE0 must be
//! enabled.
//! Further, for any particular transaction from a message object to generate
//! an interrupt, that message object must have interrupts enabled (see
//! CANMessageSet()).  \b CAN_INT_ERROR will generate an interrupt if the
//! controller enters the ``bus off'' condition, or if the error counters reach
//! a limit.  \b CAN_INT_STATUS will generate an interrupt under quite a few
//! status conditions and may provide more interrupts than the application
//! needs to handle.  When an interrupt occurs, use CANIntStatus() to determine
//! the cause.
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void
CANIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));
    ASSERT((ui32IntFlags & ~(CAN_INT_ERROR | CAN_INT_STATUS | CAN_INT_IE0 |
                             CAN_INT_IE1)) == 0);

    // Enable the specified interrupts.
    HWREG(ui32Base + CAN_O_CTL) =
        HWREG(ui32Base + CAN_O_CTL) | ui32IntFlags;
}

//*****************************************************************************
//! Enables the CAN controller.
//!
//! \param ui32Base is the base address of the CAN controller to enable.
//!
//! Enables the CAN controller for message processing.  Once enabled, the
//! controller will automatically transmit any pending frames, and process any
//! received frames.  The controller can be stopped by calling CANDisable().
//! Prior to calling CANEnable(), CANInit() should have been called to
//! initialize the controller and the CAN bus clock should be configured by
//! calling CANBitTimingSet().
//!
//! \return None.
//
//*****************************************************************************
//#pragma CODE_SECTION("ramfuncs")
void
CANEnable(uint32_t ui32Base)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));

    // Clear the init bit in the control register.
    HWREG(ui32Base + CAN_O_CTL) =
        HWREG(ui32Base + CAN_O_CTL) & ~CAN_CTL_INIT;
}

//*****************************************************************************
//! CAN Global interrupt Enable function.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables specific CAN interrupt in the global interrupt enable register
//!
//! The \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! CAN_GLB_INT_CANINT0   -Global Interrupt Enable bit for CAN INT0
//! CAN_GLB_INT_CANINT1   -Global Interrupt Enable bit for CAN INT1
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void
CANGlobalIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));
    ASSERT((ui32IntFlags & ~(CAN_GLB_INT_CANINT0 |
                             CAN_GLB_INT_CANINT1)) == 0);

    //enable the requested interrupts
    HWREG(ui32Base + CAN_O_GLB_INT_EN) = HWREG(ui32Base + CAN_O_GLB_INT_EN) |
                                         ui32IntFlags;

}

//*****************************************************************************
//! Returns the current CAN controller interrupt status.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param eIntStsReg indicates which interrupt status register to read
//!
//! Returns the value of one of two interrupt status registers.  The interrupt
//! status register read is determined by the \e eIntStsReg parameter, which
//! can have one of the following values:
//!
//! - \b CAN_INT_STS_CAUSE - indicates the cause of the interrupt
//! - \b CAN_INT_STS_OBJECT - indicates pending interrupts of all message
//! objects
//!
//! \b CAN_INT_STS_CAUSE returns the value of the controller interrupt register
//! and indicates the cause of the interrupt.  It will be a value of
//! \b CAN_INT_INT0ID_STATUS if the cause is a status interrupt.  In this case,
//! the status register should be read with the CANStatusGet() function.
//! Calling this function to read the status will also clear the status
//! interrupt.  If the value of the interrupt register is in the range 1-32,
//! then this indicates the number of the highest priority message object that
//! has an interrupt pending.  The message object interrupt can be cleared by
//! using the CANIntClear() function, or by reading the message using
//! CANMessageGet() in the case of a received message.  The interrupt handler
//! can read the interrupt status again to make sure all pending interrupts are
//! cleared before returning from the interrupt.
//!
//! \b CAN_INT_STS_OBJECT returns a bit mask indicating which message objects
//! have pending interrupts.  This can be used to discover all of the pending
//! interrupts at once, as opposed to repeatedly reading the interrupt register
//! by using \b CAN_INT_STS_CAUSE.
//!
//! \return Returns the value of one of the interrupt status registers.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
Uint32 canGetIntStatus(Uint32 canBase, tCANIntStsReg intStatusReg)
{
    uint32_t ui32Status;

    // Check the arguments.
    ASSERT(CANBaseValid(canBase));

    // See which status the caller is looking for.
    switch(intStatusReg)
    {
    // The caller wants the global interrupt status for the CAN controller
    // specified by canBase.
    case CAN_INT_STS_CAUSE:
    {
        ui32Status = HWREG(canBase + CAN_O_INT);
        break;
    }

    // The caller wants the current message status interrupt for all
    // messages.
    case CAN_INT_STS_OBJECT:
    {
        // Read message object interrupt status
        ui32Status = HWREG(canBase + CAN_O_IPEN_21);
        break;
    }

    // Request was for unknown status so just return 0.
    default:
    {
        ui32Status = 0;
        break;
    }
    }
    // Return the interrupt status value
    return(ui32Status);
}

//*****************************************************************************
//! Clears a CAN interrupt source.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32IntClr is a value indicating which interrupt source to clear.
//!
//! This function can be used to clear a specific interrupt source.  The
//! \e ui32IntClr parameter should be one of the following values:
//!
//! - \b CAN_INT_INTID_STATUS - Clears a status interrupt.
//! - 1-32 - Clears the specified message object interrupt
//!
//! It is not necessary to use this function to clear an interrupt.  This
//! should only be used if the application wants to clear an interrupt source
//! without taking the normal interrupt action.
//!
//! Normally, the status interrupt is cleared by reading the controller status
//! using CANStatusGet().  A specific message object interrupt is normally
//! cleared by reading the message object using CANMessageGet().
//!
//! \note Since there is a write buffer in the Cortex-M3 processor, it may take
//! several clock cycles before the interrupt source is actually cleared.
//! Therefore, it is recommended that the interrupt source be cleared early in
//! the interrupt handler (as opposed to the very last action) to avoid
//! returning from the interrupt handler before the interrupt source is
//! actually cleared.  Failure to do so may result in the interrupt handler
//! being immediately reentered (since NVIC still sees the interrupt source
//! asserted).
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void canIntClear(Uint32 canBase, Uint32 intClear)
{
    // Check the arguments.
    ASSERT(CANBaseValid(canBase));
    ASSERT((intClear == CAN_INT_INT0ID_STATUS) ||
           ((intClear>=1) && (intClear <=32)));

    if(intClear == CAN_INT_INT0ID_STATUS)
    {
        // Simply read and discard the status to clear the interrupt.
        HWREG(canBase + CAN_O_ES);
    }
    else
    {
        // Wait to be sure that this interface is not busy.
        while(HWREG(canBase + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
        {
        }

        // Only change the interrupt pending state by setting only the
        // CAN_IF1CMD_CLRINTPND bit.
        HWREGH(canBase + CAN_O_IF1CMD+2) = CAN_IF1CMD_CLRINTPND >> 16;

        // Send the clear pending interrupt command to the CAN controller.
        HWREGH(canBase + CAN_O_IF1CMD) = intClear & CAN_IF1CMD_MSG_NUM_M;

        // Wait to be sure that this interface is not busy.
        while(HWREG(canBase + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
        {
        }
    }
}

//*****************************************************************************
//! \internal
//!
//! Copies data from a buffer to the CAN Data registers.
//!
//! \param pucData is a pointer to the location to store the data read from the
//! CAN controller's data registers.
//! \param pui32Register is an uint32_t pointer to the first register of the
//! CAN controller's data registers.  For example, in order to use the IF1
//! register set on CAN controller 1, the value would be: \b CAN0_BASE \b +
//! \b CAN_O_IF1DA1.
//! \param iSize is the number of bytes to copy from the CAN controller.
//!
//! This function takes the steps necessary to copy data to a contiguous buffer
//! in memory from the non-contiguous data registers used by the CAN
//! controller.  This function is rarely used outside of the CANMessageGet()
//! function.
//!
//! This function replaces the original CANReadDataReg() API and performs the
//! same actions.  A macro is provided in <tt>can.h</tt> to map the original
//! API to this API.
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
static void
CANDataRegRead(unsigned char *pucData, uint32_t *pui32Register, int16_t iSize)
{
    int16_t iIdx;
    unsigned char * pucRegister = (unsigned char *) pui32Register;

    // Loop always copies 1 or 2 bytes per iteration.
    for(iIdx = 0; iIdx < iSize; iIdx++ )
    {
        // Read out the data
        pucData[iIdx] = HWREGB(pucRegister++);

    }
}

//*****************************************************************************
//! Reads a CAN message from one of the message object buffers.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32ObjID is the object number to read (1-32).
//! \param pMsgObject points to a structure containing message object fields.
//! \param bClrPendingInt indicates whether an associated interrupt should be
//! cleared.
//!
//! This function is used to read the contents of one of the 32 message objects
//! in the CAN controller, and return it to the caller.  The data returned is
//! stored in the fields of the caller-supplied structure pointed to by
//! \e pMsgObject.  The data consists of all of the parts of a CAN message,
//! plus some control and status information.
//!
//! Normally this is used to read a message object that has received and stored
//! a CAN message with a certain identifier.  However, this could also be used
//! to read the contents of a message object in order to load the fields of the
//! structure in case only part of the structure needs to be changed from a
//! previous setting.
//!
//! When using CANMessageGet, all of the same fields of the structure are
//! populated in the same way as when the CANMessageSet() function is used,
//! with the following exceptions:
//!
//! \e pMsgObject->ui32Flags:
//!
//! - \b MSG_OBJ_NEW_DATA indicates if this is new data since the last time it
//! was read
//! - \b MSG_OBJ_DATA_LOST indicates that at least one message was received on
//! this message object, and not read by the host before being overwritten.
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void
CANMessageGet(uint32_t ui32Base, uint32_t ui32ObjID, tCANMsgObject *pMsgObject,
              bool bClrPendingInt)
{
    uint32_t ui32CmdMaskReg;
    uint32_t ui32MaskReg;
    uint32_t ui32ArbReg;
    uint32_t ui32MsgCtrl;

    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));
    ASSERT((ui32ObjID <= 32) && (ui32ObjID != 0));

    // This is always a read to the Message object as this call is setting a
    // message object.
    ui32CmdMaskReg = (CAN_IF2CMD_DATA_A | CAN_IF2CMD_DATA_B |
                      CAN_IF2CMD_CONTROL | CAN_IF2CMD_MASK | CAN_IF2CMD_ARB);

    // Clear a pending interrupt and new data in a message object.
    if(bClrPendingInt)
    {
        ui32CmdMaskReg |= CAN_IF2CMD_CLRINTPND;
    }

    // Set up the request for data from the message object.
    HWREGH(ui32Base + CAN_O_IF2CMD+2) =  ui32CmdMaskReg >> 16;

    // Transfer the message object to the message object specified by ui32ObjID.
    HWREGH(ui32Base + CAN_O_IF2CMD) = ui32ObjID & CAN_IF2CMD_MSG_NUM_M;

    // Wait for busy bit to clear
    while(HWREG(ui32Base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY)
    {
    }

    // Read out the IF Registers.
    ui32MaskReg = HWREG(ui32Base + CAN_O_IF2MSK);
    ui32ArbReg = HWREG(ui32Base + CAN_O_IF2ARB);
    ui32MsgCtrl = HWREG(ui32Base + CAN_O_IF2MCTL);
    pMsgObject->ui32Flags = MSG_OBJ_NO_FLAGS;

    // Determine if this is a remote frame by checking the TXRQST and DIR bits.
    if((!(ui32MsgCtrl & CAN_IF2MCTL_TXRQST) && (ui32ArbReg & CAN_IF2ARB_DIR)) ||
       ((ui32MsgCtrl & CAN_IF2MCTL_TXRQST) && (!(ui32ArbReg & CAN_IF2ARB_DIR))))
    {
        pMsgObject->ui32Flags |= MSG_OBJ_REMOTE_FRAME;
    }

    // Get the identifier out of the register, the format depends on size of
    // the mask.
    if(ui32ArbReg & CAN_IF2ARB_XTD)
    {
        // Set the 29 bit version of the Identifier for this message object.
        pMsgObject->ui32MsgID = ui32ArbReg & CAN_IF2ARB_ID_M;

        pMsgObject->ui32Flags |= MSG_OBJ_EXTENDED_ID;
    }
    else
    {
        // The Identifier is an 11 bit value.
        pMsgObject->ui32MsgID =
            (ui32ArbReg & CAN_IF2ARB_STD_ID_M) >> CAN_IF2ARB_STD_ID_S;
    }

    // Indicate that we lost some data.
    if(ui32MsgCtrl & CAN_IF2MCTL_MSGLST)
    {
        pMsgObject->ui32Flags |= MSG_OBJ_DATA_LOST;
    }

    // Set the flag to indicate if ID masking was used.
    if(ui32MsgCtrl & CAN_IF2MCTL_UMASK)
    {
        if(ui32ArbReg & CAN_IF2ARB_XTD)
        {
            // The Identifier Mask is assumed to also be a 29 bit value.
            pMsgObject->ui32MsgIDMask = (ui32MaskReg & CAN_IF2MSK_MSK_M);

            // If this is a fully specified Mask and a remote frame then don't
            // set the MSG_OBJ_USE_ID_FILTER because the ID was not really
            // filtered.
            if((pMsgObject->ui32MsgIDMask != 0x1fffffff) ||
               ((pMsgObject->ui32Flags & MSG_OBJ_REMOTE_FRAME) == 0))
            {
                pMsgObject->ui32Flags |= MSG_OBJ_USE_ID_FILTER;
            }
        }
        else
        {
            // The Identifier Mask is assumed to also be an 11 bit value.
            pMsgObject->ui32MsgIDMask = ((ui32MaskReg & CAN_IF2MSK_MSK_M) >>
                                       18);

            // If this is a fully specified Mask and a remote frame then don't
            // set the MSG_OBJ_USE_ID_FILTER because the ID was not really
            // filtered.
            if((pMsgObject->ui32MsgIDMask != 0x7ff) ||
               ((pMsgObject->ui32Flags & MSG_OBJ_REMOTE_FRAME) == 0))
            {
                pMsgObject->ui32Flags |= MSG_OBJ_USE_ID_FILTER;
            }
        }

        // Indicate if the extended bit was used in filtering.
        if(ui32MaskReg & CAN_IF2MSK_MXTD)
        {
            pMsgObject->ui32Flags |= MSG_OBJ_USE_EXT_FILTER;
        }

        // Indicate if direction filtering was enabled.
        if(ui32MaskReg & CAN_IF2MSK_MDIR)
        {
            pMsgObject->ui32Flags |= MSG_OBJ_USE_DIR_FILTER;
        }
    }

    // Set the interrupt flags.
    if(ui32MsgCtrl & CAN_IF2MCTL_TXIE)
    {
        pMsgObject->ui32Flags |= MSG_OBJ_TX_INT_ENABLE;
    }
    if(ui32MsgCtrl & CAN_IF2MCTL_RXIE)
    {
        pMsgObject->ui32Flags |= MSG_OBJ_RX_INT_ENABLE;
    }

    // See if there is new data available.
    if(ui32MsgCtrl & CAN_IF2MCTL_NEWDAT)
    {
        // Get the amount of data needed to be read.
        pMsgObject->ui32MsgLen = (ui32MsgCtrl & CAN_IF2MCTL_DLC_M);

        // Don't read any data for a remote frame, there is nothing valid in
        // that buffer anyway.
        if((pMsgObject->ui32Flags & MSG_OBJ_REMOTE_FRAME) == 0)
        {
            // Read out the data from the CAN registers.
            CANDataRegRead(pMsgObject->pucMsgData,
                           (uint32_t *)(ui32Base + CAN_O_IF2DATA),
                           pMsgObject->ui32MsgLen);
        }

        // Now clear out the new data flag.
        HWREG(ui32Base + CAN_O_IF2CMD + 2) = CAN_IF2CMD_TXRQST >> 16;

        // Transfer the message object to the message object specified by
        // ui32ObjID.
        HWREGH(ui32Base + CAN_O_IF2CMD) = ui32ObjID & CAN_IF2CMD_MSG_NUM_M;

        // Wait for busy bit to clear
        while(HWREG(ui32Base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY)
        {
        }

        // Indicate that there is new data in this message.
        pMsgObject->ui32Flags |= MSG_OBJ_NEW_DATA;
    }
    else
    {
        // Along with the MSG_OBJ_NEW_DATA not being set the amount of data
        // needs to be set to zero if none was available.
        pMsgObject->ui32MsgLen = 0;
    }
}

//*****************************************************************************
//! \internal
//!
//! Copies data from a buffer to the CAN Data registers.
//!
//! \param pucData is a pointer to the data to be written out to the CAN
//! controller's data registers.
//! \param pui32Register is an uint32_t pointer to the first register of the
//! CAN controller's data registers.  For example, in order to use the IF1
//! register set on CAN controller 0, the value would be: \b CAN0_BASE \b +
//! \b CAN_O_IF1DA1.
//! \param iSize is the number of bytes to copy into the CAN controller.
//!
//! This function takes the steps necessary to copy data from a contiguous
//! buffer in memory into the non-contiguous data registers used by the CAN
//! controller.  This function is rarely used outside of the CANMessageSet()
//! function.
//!
//! This function replaces the original CANWriteDataReg() API and performs the
//! same actions.  A macro is provided in <tt>can.h</tt> to map the original
//! API to this API.
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
static void
CANDataRegWrite(unsigned char *pucData, uint32_t *pui32Register, int16_t iSize)
{
    int16_t iIdx;
    unsigned char * pucRegister = (unsigned char *) pui32Register;

    // Loop always copies 1 or 2 bytes per iteration.
    for(iIdx = 0; iIdx < iSize; iIdx++ )
    {

        // Write out the data 8 bits at a time.
        HWREGB(pucRegister++) = pucData[iIdx];

    }
}

//*****************************************************************************
//! Reads one of the controller status registers.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param eStatusReg is the status register to read.
//!
//! Reads a status register of the CAN controller and returns it to the caller.
//! The different status registers are:
//!
//! - \b CAN_STS_CONTROL - the main controller status
//! - \b CAN_STS_TXREQUEST - bit mask of objects pending transmission
//! - \b CAN_STS_NEWDAT - bit mask of objects with new data
//! - \b CAN_STS_MSGVAL - bit mask of objects with valid configuration
//!
//! When reading the main controller status register, a pending status
//! interrupt will be cleared.  This should be used in the interrupt handler
//! for the CAN controller if the cause is a status interrupt.  The controller
//! status register fields are as follows:
//!
//! - \b CAN_STATUS_PDA - controller in local power down mode
//! - \b CAN_STATUS_WAKE_UP - controller initiated system wake up
//! - \b CAN_STATUS_PERR - parity error detected
//! - \b CAN_STATUS_BUS_OFF - controller is in bus-off condition
//! - \b CAN_STATUS_EWARN - an error counter has reached a limit of at least 96
//! - \b CAN_STATUS_EPASS - CAN controller is in the error passive state
//! - \b CAN_STATUS_RXOK - a message was received successfully (independent of
//! any message filtering).
//! - \b CAN_STATUS_TXOK - a message was successfully transmitted
//! - \b CAN_STATUS_LEC_NONE - no error
//! - \b CAN_STATUS_LEC_STUFF - stuffing error detected
//! - \b CAN_STATUS_LEC_FORM - a format error occurred in the fixed format part
//! of a message
//! - \b CAN_STATUS_LEC_ACK - a transmitted message was not acknowledged
//! - \b CAN_STATUS_LEC_BIT1 - dominant level detected when trying to send in
//! recessive mode
//! - \b CAN_STATUS_LEC_BIT0 - recessive level detected when trying to send in
//! dominant mode
//! - \b CAN_STATUS_LEC_CRC - CRC error in received message
//!
//! The remaining status registers are 32-bit bit maps to the message objects.
//! They can be used to quickly obtain information about the status of all the
//! message objects without needing to query each one.  They contain the
//! following information:
//!
//! - \b CAN_STS_TXREQUEST - if a message object's TxRequest bit is set, that
//! means that a transmission is pending on that object.  The application can
//! use this to determine which objects are still waiting to send a message.
//! - \b CAN_STS_NEWDAT - if a message object's NewDat bit is set, that means
//! that a new message has been received in that object, and has not yet been
//! picked up by the host application
//! - \b CAN_STS_MSGVAL - if a message object's MsgVal bit is set, that means
//! it has a valid configuration programmed.  The host application can use this
//! to determine which message objects are empty/unused.
//!
//! \return Returns the value of the status register.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
Uint32 canGetStatus(Uint32 canBase, tCANStsReg statusReg)
{
    uint32_t ui32Status;

    // Check the arguments.
    ASSERT(CANBaseValid(canBase));

    switch(statusReg)
    {
    // Just return the global CAN status register since that is what was
    // requested.
    case CAN_STS_CONTROL:
    {
        ui32Status = HWREG(canBase + CAN_O_ES);
        break;
    }

    // Return objects with valid transmit requests
    case CAN_STS_TXREQUEST:
    {
        ui32Status = HWREG(canBase + CAN_O_TXRQ_21);
        break;
    }

    // Return messages objects with new data
    case CAN_STS_NEWDAT:
    {
        ui32Status = HWREG(canBase + CAN_O_NDAT_21);
        break;
    }

    // Return valid message objects
    case CAN_STS_MSGVAL:
    {
        ui32Status = HWREG(canBase + CAN_O_MVAL_21);
        break;
    }

    // Unknown CAN status requested so return 0.
    default:
    {
        ui32Status = 0;
        break;
    }
    }
    return(ui32Status);
}

//*****************************************************************************
//! Configures a message object in the CAN controller.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32ObjID is the object number to configure (1-32).
//! \param pMsgObject is a pointer to a structure containing message object
//! settings.
//! \param eMsgType indicates the type of message for this object.
//!
//! This function is used to configure any one of the 32 message objects in the
//! CAN controller.  A message object can be configured as any type of CAN
//! message object as well as several options for automatic transmission and
//! reception.  This call also allows the message object to be configured to
//! generate interrupts on completion of message receipt or transmission.  The
//! message object can also be configured with a filter/mask so that actions
//! are only taken when a message that meets certain parameters is seen on the
//! CAN bus.
//!
//! The \e eMsgType parameter must be one of the following values:
//!
//! - \b MSG_OBJ_TYPE_TX - CAN transmit message object.
//! - \b MSG_OBJ_TYPE_TX_REMOTE - CAN transmit remote request message object.
//! - \b MSG_OBJ_TYPE_RX - CAN receive message object.
//! - \b MSG_OBJ_TYPE_RX_REMOTE - CAN receive remote request message object.
//! - \b MSG_OBJ_TYPE_RXTX_REMOTE - CAN remote frame receive remote, then
//! transmit message object.
//!
//! The message object pointed to by \e pMsgObject must be populated by the
//! caller, as follows:
//!
//! - \e ui32MsgID - contains the message ID, either 11 or 29 bits.
//! - \e ui32MsgIDMask - mask of bits from \e ui32MsgID that must match if
//! identifier filtering is enabled.
//! - \e ui32Flags
//!   - Set \b MSG_OBJ_TX_INT_ENABLE flag to enable interrupt on transmission.
//!   - Set \b MSG_OBJ_RX_INT_ENABLE flag to enable interrupt on receipt.
//!   - Set \b MSG_OBJ_USE_ID_FILTER flag to enable filtering based on the
//!   identifier mask specified by \e ui32MsgIDMask.
//! - \e ui32MsgLen - the number of bytes in the message data.  This should be
//! non-zero even for a remote frame; it should match the expected bytes of the
//! data responding data frame.
//! - \e pucMsgData - points to a buffer containing up to 8 bytes of data for a
//! data frame.
//!
//! \b Example: To send a data frame or remote frame(in response to a remote
//! request), take the following steps:
//!
//! -# Set \e eMsgType to \b MSG_OBJ_TYPE_TX.
//! -# Set \e pMsgObject->ui32MsgID to the message ID.
//! -# Set \e pMsgObject->ui32Flags. Make sure to set \b MSG_OBJ_TX_INT_ENABLE to
//! allow an interrupt to be generated when the message is sent.
//! -# Set \e pMsgObject->ui32MsgLen to the number of bytes in the data frame.
//! -# Set \e pMsgObject->pucMsgData to point to an array containing the bytes
//! to send in the message.
//! -# Call this function with \e ui32ObjID set to one of the 32 object buffers.
//!
//! \b Example: To receive a specific data frame, take the following steps:
//!
//! -# Set \e eMsgObjType to \b MSG_OBJ_TYPE_RX.
//! -# Set \e pMsgObject->ui32MsgID to the full message ID, or a partial mask to
//! use partial ID matching.
//! -# Set \e pMsgObject->ui32MsgIDMask bits that should be used for masking
//! during comparison.
//! -# Set \e pMsgObject->ui32Flags as follows:
//!   - Set \b MSG_OBJ_TX_INT_ENABLE flag to be interrupted when the data frame
//!   is received.
//!   - Set \b MSG_OBJ_USE_ID_FILTER flag to enable identifier based filtering.
//! -# Set \e pMsgObject->ui32MsgLen to the number of bytes in the expected data
//! frame.
//! -# The buffer pointed to by \e pMsgObject->pucMsgData  and
//! \e pMsgObject->ui32MsgLen are not used by this call as no data is present at
//! the time of the call.
//! -# Call this function with \e ui32ObjID set to one of the 32 object buffers.
//!
//! If you specify a message object buffer that already contains a message
//! definition, it will be overwritten.
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void
CANMessageSet(uint32_t ui32Base, uint32_t ui32ObjID, tCANMsgObject *pMsgObject,
              tMsgObjType eMsgType)
{
    uint32_t ui32CmdMaskReg;
    uint32_t ui32MaskReg;
    uint32_t ui32ArbReg;
    uint32_t ui32MsgCtrl;
    bool bTransferData;
    bool bUseExtendedID;

    bTransferData = 0;

    // Check the arguments.
    ASSERT(CANBaseValid(ui32Base));
    ASSERT((ui32ObjID <= 32) && (ui32ObjID != 0));
    ASSERT((eMsgType == MSG_OBJ_TYPE_TX) ||
           (eMsgType == MSG_OBJ_TYPE_TX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_RX) ||
           (eMsgType == MSG_OBJ_TYPE_RX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_TX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_RXTX_REMOTE));

    // Wait for busy bit to clear
    while(HWREG(ui32Base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY)
    {
    }

    // See if we need to use an extended identifier or not.
    if((pMsgObject->ui32MsgID > CAN_MAX_11BIT_MSG_ID) ||
       (pMsgObject->ui32Flags & MSG_OBJ_EXTENDED_ID))
    {
        bUseExtendedID = 1;
    }
    else
    {
        bUseExtendedID = 0;
    }

    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    ui32CmdMaskReg = (CAN_IF1CMD_DIR | CAN_IF1CMD_DATA_A | CAN_IF1CMD_DATA_B |
                      CAN_IF1CMD_CONTROL);

    // Initialize the values to a known state before filling them in based on
    // the type of message object that is being configured.
    ui32ArbReg = 0;
    ui32MsgCtrl = 0;
    ui32MaskReg = 0;

    switch(eMsgType)
    {
    // Transmit message object.
    case MSG_OBJ_TYPE_TX:
    {
        // Set the TXRQST bit and the reset the rest of the register.
        ui32MsgCtrl |= CAN_IF1MCTL_TXRQST;
        ui32ArbReg = CAN_IF1ARB_DIR;
        bTransferData = 1;
        break;
    }

    // Transmit remote request message object
    case MSG_OBJ_TYPE_TX_REMOTE:
    {
        // Set the TXRQST bit and the reset the rest of the register.
        ui32MsgCtrl |= CAN_IF1MCTL_TXRQST;
        ui32ArbReg = 0;
        break;
    }

    // Receive message object.
    case MSG_OBJ_TYPE_RX:
    {
        // This clears the DIR bit along with everything else.  The TXRQST
        // bit was cleared by defaulting ui32MsgCtrl to 0.
        ui32ArbReg = 0;
        break;
    }

    // Receive remote request message object.
    case MSG_OBJ_TYPE_RX_REMOTE:
    {
        // The DIR bit is set to one for remote receivers.  The TXRQST bit
        // was cleared by defaulting ui32MsgCtrl to 0.
        ui32ArbReg = CAN_IF1ARB_DIR;

        // Set this object so that it only indicates that a remote frame
        // was received and allow for software to handle it by sending back
        // a data frame.
        ui32MsgCtrl = CAN_IF1MCTL_UMASK;

        // Use the full Identifier by default.
        ui32MaskReg = CAN_IF1MSK_MSK_M;

        // Make sure to send the mask to the message object.
        ui32CmdMaskReg |= CAN_IF1CMD_MASK;
        break;
    }

    // Remote frame receive remote, with auto-transmit message object.
    case MSG_OBJ_TYPE_RXTX_REMOTE:
    {
        // Oddly the DIR bit is set to one for remote receivers.
        ui32ArbReg = CAN_IF1ARB_DIR;

        // Set this object to auto answer if a matching identifier is seen.
        ui32MsgCtrl = CAN_IF1MCTL_RMTEN | CAN_IF1MCTL_UMASK;

        // The data to be returned needs to be filled in.
        bTransferData = 1;
        break;
    }

    // This case should never happen due to the ASSERT statement at the
    // beginning of this function.
    default:
    {
        return;
    }
    }

    // Configure the Mask Registers.
    if(pMsgObject->ui32Flags & MSG_OBJ_USE_ID_FILTER)
    {
        if(bUseExtendedID)
        {
            // Set the 29 bits of Identifier mask that were requested.
            ui32MaskReg = pMsgObject->ui32MsgIDMask & CAN_IF1MSK_MSK_M;

        }
        else
        {

            // Put the 11 bit Mask Identifier into the upper bits of the field
            // in the register.
            ui32MaskReg = ((pMsgObject->ui32MsgIDMask << CAN_IF1ARB_STD_ID_S) &
                         CAN_IF1ARB_STD_ID_M);
        }
    }

    // If the caller wants to filter on the extended ID bit then set it.
    if((pMsgObject->ui32Flags & MSG_OBJ_USE_EXT_FILTER) ==
       MSG_OBJ_USE_EXT_FILTER)
    {
        ui32MaskReg |= CAN_IF1MSK_MXTD;
    }

    // The caller wants to filter on the message direction field.
    if((pMsgObject->ui32Flags & MSG_OBJ_USE_DIR_FILTER) ==
       MSG_OBJ_USE_DIR_FILTER)
    {
        ui32MaskReg |= CAN_IF1MSK_MDIR;
    }

    if(pMsgObject->ui32Flags & (MSG_OBJ_USE_ID_FILTER | MSG_OBJ_USE_DIR_FILTER |
                              MSG_OBJ_USE_EXT_FILTER))
    {
        // Set the UMASK bit to enable using the mask register.
        ui32MsgCtrl |= CAN_IF1MCTL_UMASK;

        // Set the MASK bit so that this gets transferred to the Message
        // Object.
        ui32CmdMaskReg |= CAN_IF1CMD_MASK;
    }

    // Set the Arb bit so that this gets transferred to the Message object.
    ui32CmdMaskReg |= CAN_IF1CMD_ARB;

    // Configure the Arbitration registers.
    if(bUseExtendedID)
    {
        // Set the 29 bit version of the Identifier for this message object.
        // Mark the message as valid and set the extended ID bit.
        ui32ArbReg |=
            (pMsgObject->ui32MsgID & CAN_IF1ARB_ID_M) |
            CAN_IF1ARB_MSGVAL | CAN_IF1ARB_XTD;

    }
    else
    {
        // Set the 11 bit version of the Identifier for this message object.
        // The lower 18 bits are set to zero.
        // Mark the message as valid.
        ui32ArbReg |=
            ((pMsgObject->ui32MsgID <<
              CAN_IF1ARB_STD_ID_S) & CAN_IF1ARB_STD_ID_M)
            | CAN_IF1ARB_MSGVAL;

    }

    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    ui32MsgCtrl |= (pMsgObject->ui32MsgLen & CAN_IF1MCTL_DLC_M);

    // Mark this as the last entry if this is not the last entry in a FIFO.
    if((pMsgObject->ui32Flags & MSG_OBJ_FIFO) == 0)
    {
        ui32MsgCtrl |= CAN_IF1MCTL_EOB;
    }

    // Enable transmit interrupts if they should be enabled.
    if(pMsgObject->ui32Flags & MSG_OBJ_TX_INT_ENABLE)
    {
        ui32MsgCtrl |= CAN_IF1MCTL_TXIE;
    }

    // Enable receive interrupts if they should be enabled.
    if(pMsgObject->ui32Flags & MSG_OBJ_RX_INT_ENABLE)
    {
        ui32MsgCtrl |= CAN_IF1MCTL_RXIE;
    }

    // Write the data out to the CAN Data registers if needed.
    if(bTransferData)
    {
        CANDataRegWrite(pMsgObject->pucMsgData,
                        (uint32_t *)(ui32Base + CAN_O_IF1DATA),
                        pMsgObject->ui32MsgLen);
    }

    // Write out the registers to program the message object.
    HWREGH(ui32Base + CAN_O_IF1CMD+2) = ui32CmdMaskReg >> 16;
    HWREG(ui32Base + CAN_O_IF1MSK) = ui32MaskReg;
    HWREG(ui32Base + CAN_O_IF1ARB) = ui32ArbReg;
    HWREG(ui32Base + CAN_O_IF1MCTL) = ui32MsgCtrl;

    // Transfer the message object to the message object specifiec by ui32ObjID.
    HWREGH(ui32Base + CAN_O_IF1CMD) = ui32ObjID & CAN_IF1CMD_MSG_NUM_M;

    return;
}

//*****************************************************************************
//! CAN Global interrupt Clear function.
//!
//! \param ui32Base is the base address of the CAN controller.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Clear the specific CAN interrupt bit i nthe global interrupt flag register.
//!
//! The \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! CAN_GLB_INT_CANINT0   -Global Interrupt bit for CAN INT0
//! CAN_GLB_INT_CANINT1   -Global Interrupt bit for CAN INT1
//!
//! \return None.
//
//*****************************************************************************
#pragma CODE_SECTION("ramfuncs")
void canGlobalIntClear(Uint32 canBase, Uint32 intFlags)
{
    // Check the arguments.
    ASSERT(CANBaseValid(canBase));
    ASSERT((intFlags & ~(CAN_GLB_INT_CANINT0 |
                             CAN_GLB_INT_CANINT1)) == 0);

    //enable the requested interrupts
    HWREG(canBase + CAN_O_GLB_INT_CLR) = intFlags;

}

//******************************************************************************
// Functions for external use
//******************************************************************************

//#pragma CODE_SECTION("ramfuncs")
void canInit(tCANMsgObject *canRcvMessage)
{
	// Initialize the CAN controller
	CANInit(CANA_BASE);

	// Setup CAN to be clocked off the PLL output clock
	CANClkSourceSelect(CANA_BASE, 0);   /* 500kHz CAN-Clock */

	// Set up the bit rate for the CAN bus.  This function sets up the CAN
	// bus timing for a nominal configuration.  You can achieve more control
	// over the CAN bus timing by using the function CANBitTimingSet() instead
	// of this one, if needed.
	// In this example, the CAN bus is set to 500 kHz.  In the function below,
	// the call to SysCtlClockGet() is used to determine the clock rate that
	// is used for clocking the CAN peripheral.  This can be replaced with a
	// fixed value if you know the value of the system clock, saving the extra
	// function call.  For some parts, the CAN peripheral is clocked by a fixed
	// 8 MHz regardless of the system clock in which case the call to
	// SysCtlClockGet() should be replaced with 8000000.  Consult the data
	// sheet for more information about CAN peripheral clocking.
	CANBitRateSet(CANA_BASE, 200000000, 500000);

	// Enable interrupts on the CAN peripheral.  This example uses static
	// allocation of interrupt handlers which means the name of the handler
	// is in the vector table of startup code.  If you want to use dynamic
	// allocation of the vector table, then you must also call CANIntRegister()
	// here.
	CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

	// Enable the CAN for operation.
	CANEnable(CANA_BASE);

	CANGlobalIntEnable(CANA_BASE, CAN_GLB_INT_CANINT0);
	
	CANMessageSet(CANA_BASE, 2, canRcvMessage, MSG_OBJ_TYPE_RX);
}

#pragma CODE_SECTION("ramfuncs")
void canRead(tCANMsgObject &canMessage)
{
	CANMessageGet(CANA_BASE, 2, &canMessage, true);
}

#pragma CODE_SECTION("ramfuncs")
void canWrite(tCANMsgObject *canMessage)
{
	CANMessageSet(CANA_BASE, 1, canMessage, MSG_OBJ_TYPE_TX);
}

