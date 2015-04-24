#ifndef SPI_H_
#define SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_Cla_typedefs.h"  // F2837xD CLA Type definitions

#ifdef __cplusplus
}
#endif

#define SPI_STATUS			int32
#define SPI_NO_ERROR		0
#define SPI_RX_TIMEOUT		-1
#define SPI_FIFO_OVERFLOW	-2

#define SPI_FIFO_LENGTH		16

void spiInit();
SPI_STATUS spiXfer(Uint16 numBytes, Uint8 *data_out, Uint8 *data_in);

#endif /* SPI_H_ */
