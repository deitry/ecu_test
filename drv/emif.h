#ifndef EMIF_H_
#define EMIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xD_Cla_typedefs.h"  // F2837xD CLA Type definitions

#ifdef __cplusplus
}
#endif

void emifInit();
void emifWrite(Uint16 address, Uint16 data);
Uint16 emifRead(Uint16 address);

#endif /* EMIF_H_ */
