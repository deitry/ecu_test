#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_Cla_typedefs.h"  // F2837xD CLA Type definitions

#ifdef __cplusplus
}
#endif

#define I2C_STATUS			int32
#define I2C_ERR_NO_ERROR	0
#define I2C_ERR_STOP_BIT	-1
#define I2C_ERR_BUS_BUSY	-2
#define I2C_ERR_ARDY		-3
#define I2C_ERR_SCD			-4

void i2cInit();
I2C_STATUS i2cWrite(Uint16 address, Uint16 size, Uint8 *data, Uint8 stopCondBit);
I2C_STATUS i2cRead(Uint16 address, Uint16 size, Uint8 *data);

#endif /* I2C_H_ */
