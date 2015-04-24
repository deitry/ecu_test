/*
 * can_if.h
 *
 *  Created on: 24  эт. 2015 у.
 *      Author: ┬ютр
 */

#ifndef CAN_IF_H_
#define CAN_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "F2837xD_Cla_typedefs.h"  // F2837xD CLA Type definitions
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_can.h"

#ifdef __cplusplus
}
#endif

#define CAN_STATUS	int32
#define CAN_NO_ERROR	0

//*****************************************************************************
// These are the flags used by the tCANMsgObject.ui32Flags value when calling the
// CANMessageSet() and CANMessageGet() functions.
//*****************************************************************************

//! This definition is used with the tCANMsgObject ui32Flags value and indicates
//! that transmit interrupts should be enabled, or are enabled.
#define MSG_OBJ_TX_INT_ENABLE           0x00000001

//! This indicates that receive interrupts should be enabled, or are
//! enabled.
#define MSG_OBJ_RX_INT_ENABLE           0x00000002

//! This indicates that a message object will use or is using an extended
//! identifier.
#define MSG_OBJ_EXTENDED_ID             0x00000004

//! This indicates that a message object will use or is using filtering
//! based on the object's message identifier.
#define MSG_OBJ_USE_ID_FILTER           0x00000008

//! This indicates that new data was available in the message object.
#define MSG_OBJ_NEW_DATA                0x00000080

//! This indicates that data was lost since this message object was last
//! read.
#define MSG_OBJ_DATA_LOST               0x00000100

//! This indicates that a message object will use or is using filtering
//! based on the direction of the transfer.  If the direction filtering is
//! used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_DIR_FILTER          (0x00000010 | MSG_OBJ_USE_ID_FILTER)

//! This indicates that a message object will use or is using message
//! identifier filtering based on the extended identifier.  If the extended
//! identifier filtering is used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_EXT_FILTER          (0x00000020 | MSG_OBJ_USE_ID_FILTER)

//! This indicates that a message object is a remote frame.
#define MSG_OBJ_REMOTE_FRAME            0x00000040

//! This indicates that this message object is part of a FIFO structure and
//! not the final message object in a FIFO.
#define MSG_OBJ_FIFO                    0x00000200

//! This indicates that a message object has no flags set.
#define MSG_OBJ_NO_FLAGS                0x00000000

//*****************************************************************************
//! The structure used for encapsulating all the items associated with a CAN
//! message object in the CAN controller.
//*****************************************************************************
typedef struct
{
        //! The CAN message identifier used for 11 or 29 bit identifiers.
        uint32_t ui32MsgID;

        //! The message identifier mask used when identifier filtering is enabled.
        uint32_t ui32MsgIDMask;

        //! This value holds various status flags and settings specified by
        //! tCANObjFlags.
        uint32_t ui32Flags;

        //! This value is the number of bytes of data in the message object.
        uint32_t ui32MsgLen;

        //! This is a pointer to the message object's data.
        unsigned char *pucMsgData;
}
tCANMsgObject;

//*****************************************************************************
//! This data type is used to identify the interrupt status register.  This is
//! used when calling the CANIntStatus() function.
//*****************************************************************************
typedef enum
{
        //! Read the CAN interrupt status information.
        CAN_INT_STS_CAUSE,

        //! Read a message object's interrupt status.
        CAN_INT_STS_OBJECT
}
tCANIntStsReg;

//*****************************************************************************
//! This data type is used to identify which of several status registers to
//! read when calling the CANStatusGet() function.
//*****************************************************************************
typedef enum
{
        //! Read the full CAN controller status.
        CAN_STS_CONTROL,

        //! Read the full 32-bit mask of message objects with a transmit request
        //! set.
        CAN_STS_TXREQUEST,

        //! Read the full 32-bit mask of message objects with new data available.
        CAN_STS_NEWDAT,

        //! Read the full 32-bit mask of message objects that are enabled.
        CAN_STS_MSGVAL
}
tCANStsReg;

//*****************************************************************************
// These definitions are used to specify interrupt sources to CANIntEnable()
// and CANIntDisable().
//*****************************************************************************
//! This flag is used to allow a CAN controller to generate error
//! interrupts.
#define CAN_INT_ERROR           0x00000008

//! This flag is used to allow a CAN controller to generate status
//! interrupts.
#define CAN_INT_STATUS          0x00000004

//! This flag is used to allow a CAN controller to generate interrupts
//! on interrupt line 0
#define CAN_INT_IE0             0x00000002

//! This flag is used to allow a CAN controller to generate interrupts
//! on interrupt line 1
#define CAN_INT_IE1             0x00020000

// Defined to maintain compatability with Stellaris Examples
#define CAN_INT_MASTER          CAN_INT_IE0

//*****************************************************************************
// These definitions are used to specify the clock source to
// CANClkSourceSelect()
//*****************************************************************************
//! This flag is used to clock the CAN controller Selected CPU SYSCLKOUT
//! (CPU1.Sysclk or CPU2.Sysclk).
#define CAN_CLK_CPU_SYSCLKOUT       0 // PERx.SYSCLK (default on reset)

//! This flag is used to clock the CAN controller with the X1/X2 oscillator
//! clock.
#define CAN_CLK_EXT_OSC             1 // External Oscillator (XTAL)

//! This flag is used to clock the CAN controller with the clock from
//! AUXCLKIN (from GPIO)
#define CAN_CLK_AUXCLKIN            2 // AUXCLKIN (from GPIO)

//*****************************************************************************
//! This definition is used to determine the type of message object that will
//! be set up via a call to the CANMessageSet() API.
//*****************************************************************************
typedef enum
{
        //! Transmit message object.
        MSG_OBJ_TYPE_TX,

        //! Transmit remote request message object
        MSG_OBJ_TYPE_TX_REMOTE,

        //! Receive message object.
        MSG_OBJ_TYPE_RX,

        //! Receive remote request message object.
        MSG_OBJ_TYPE_RX_REMOTE,

        //! Remote frame receive remote, with auto-transmit message object.
        MSG_OBJ_TYPE_RXTX_REMOTE
}
tMsgObjType;

//*****************************************************************************
// The following enumeration contains all error or status indicators that can
// be returned when calling the CANStatusGet() function.
//*****************************************************************************
//! CAN controller is in local power down mode.
#define CAN_STATUS_PDA                0x00000400

//! CAN controller has initiated a system wakeup.
#define CAN_STATUS_WAKE_UP            0x00000200

//! CAN controller has detected a parity error.
#define CAN_STATUS_PERR               0x00000100

//! CAN controller has entered a Bus Off state.
#define CAN_STATUS_BUS_OFF            0x00000080

//! CAN controller error level has reached warning level.
#define CAN_STATUS_EWARN              0x00000040

//! CAN controller error level has reached error passive level.
#define CAN_STATUS_EPASS              0x00000020

//! A message was received successfully since the last read of this status.
#define CAN_STATUS_RXOK               0x00000010

//! A message was transmitted successfully since the last read of this
//! status.
#define CAN_STATUS_TXOK               0x00000008

//! This is the mask for the last error code field.
#define CAN_STATUS_LEC_MSK            0x00000007

//! There was no error.
#define CAN_STATUS_LEC_NONE           0x00000000

//! A bit stuffing error has occurred.
#define CAN_STATUS_LEC_STUFF          0x00000001

//! A formatting error has occurred.
#define CAN_STATUS_LEC_FORM           0x00000002

//! An acknowledge error has occurred.
#define CAN_STATUS_LEC_ACK            0x00000003

//! The bus remained a bit level of 1 for longer than is allowed.
#define CAN_STATUS_LEC_BIT1           0x00000004

//! The bus remained a bit level of 0 for longer than is allowed.
#define CAN_STATUS_LEC_BIT0           0x00000005

//! A CRC error has occurred.
#define CAN_STATUS_LEC_CRC            0x00000006

//*****************************************************************************
// The following macros are aded for the new Global Interrupt EN/FLG/CLR
// register
//*****************************************************************************
//CANINT0 global interrupt bit
#define CAN_GLOBAL_INT_CANINT0       0x00000001

//CANINT1 global interrupt bit
#define CAN_GLOBAL_INT_CANINT1       0x00000002

//*****************************************************************************
// The following macros are missing in hw_can.h because of scripting
// but driverlib can.c needs them
//*****************************************************************************

#define CAN_INT_INT0ID_STATUS       0x8000 /**/

#define CAN_IF1ARB_STD_ID_S         18
#define CAN_IF1ARB_STD_ID_M         0x1FFC0000 // Standard Message Identifier

#define CAN_IF2ARB_STD_ID_S         18
#define CAN_IF2ARB_STD_ID_M         0x1FFC0000 // Standard Message Identifier


void canInit(tCANMsgObject *canRcvMessage);
void	canRead(tCANMsgObject &canMessage);
void	canWrite(tCANMsgObject *canMessage);
Uint32 canGetIntStatus(Uint32 canBase, tCANIntStsReg intStatusReg);
Uint32 canGetStatus(Uint32 canBase, tCANStsReg statusReg);
void canIntClear(Uint32 canBase, Uint32 intClear);
void canGlobalIntClear(Uint32 canBase, Uint32 intFlags);


#endif /* CAN_IF_H_ */
