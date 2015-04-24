#ifndef DIESEL_DRV_H_
#define DIESEL_DRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <F2837xD_Cla_typedefs.h>  // F2837xD CLA Type definitions

#ifdef __cplusplus
}
#endif

// Коды ошибок
#define DIESEL_STATUS					int32
#define DIESEL_ERR_NO_ERROR				0
#define DIESEL_ERR_ILLEGAL_SENSOR		-1
#define DIESEL_ERR_SPI_ERROR			-2
#define DIESEL_ERR_I2C_ERROR			-3

// Описание датчиков
#define TEMPERATURE_SENSOR	0x0001
#define CURRENT_SENSOR		0x0002
#define ANALOG_INPUT		0x0004
#define POTENTIO_SENSOR		0x0004
#define ANALOG_OUTPUT		0x0008
#define PRESSURE_SENSOR		0x0010
#define HALL_SENSOR			0x0005
#define INDUCTIVE_SENSOR	0x0006
#define INJECTOR_SENSOR		0x0007

// Описание каналов датчиков
#define TEMP_SENS_CHANNEL_1	0x00
#define TEMP_SENS_CHANNEL_2	0x01
#define TEMP_SENS_CHANNEL_3	0x02
#define TEMP_SENS_CHANNEL_4	0x03
#define TEMP_SENS_CHANNEL_5	0x04
#define TEMP_SENS_CHANNEL_6	0x05
#define TEMP_SENS_CHANNEL_7	0x06
#define TEMP_SENS_CHANNEL_8	0x07

#define CURR_SENS_CHANNEL_1	0x00
#define CURR_SENS_CHANNEL_2	0x01
#define CURR_SENS_CHANNEL_3	0x02
#define CURR_SENS_CHANNEL_4	0x03
#define CURR_SENS_CHANNEL_5	0x04
#define CURR_SENS_CHANNEL_6	0x05
#define CURR_SENS_CHANNEL_7	0x06
#define CURR_SENS_CHANNEL_8	0x07

#define ANALOG_IN_CHANNEL_1	0x00
#define ANALOG_IN_CHANNEL_2	0x01
#define ANALOG_IN_CHANNEL_3	0x02
#define ANALOG_IN_CHANNEL_4	0x03

#define POT_SENS_CHANNEL_1	0x04
#define POT_SENS_CHANNEL_2	0x05
#define POT_SENS_CHANNEL_3	0x06
#define POT_SENS_CHANNEL_4	0x07

#define ANALOG_OUT_CHANNEL_1	0x01
#define ANALOG_OUT_CHANNEL_2	0x02
#define ANALOG_OUT_CHANNEL_3	0x04
#define ANALOG_OUT_CHANNEL_4	0x08

// Описание каналов форсунок
#define INJECTOR_CHANNEL_1		0x0001
#define INJECTOR_CHANNEL_2		0x0002
#define INJECTOR_CHANNEL_3		0x0004
#define INJECTOR_CHANNEL_4		0x0008
#define INJECTOR_CHANNEL_5		0x0010
#define INJECTOR_CHANNEL_6		0x0020
#define INJECTOR_CHANNEL_7		0x0040
#define INJECTOR_CHANNEL_8		0x0080
#define INJECTOR_CHANNEL_9		0x0100
#define INJECTOR_CHANNEL_10		0x0200
#define INJECTOR_CHANNEL_11		0x0400
#define INJECTOR_CHANNEL_12		0x0800

// Маски для датчиков Холла
#define HALL_SENSOR_1_MASK		0x0001
#define HALL_SENSOR_2_MASK		0x0002
#define HALL_SENSOR_3_MASK		0x0004

// Маска для индуктивного датчика
#define INDUCTIVE_SENSOR_MASK	0x0001

// LEDs
#define LED_RED_1	0x01
#define LED_YELLOW	0x02
#define LED_GREEN	0x04
#define LED_RED_2	0x08

void dieselDrvInit();
void setInjector(Uint16 step1TimeUs, Uint16 step2TimeUs, Uint8 dutyCycle1, Uint8 dutyCycle2);
void startInjector(Uint16 injectorNum);
DIESEL_STATUS setSensor(Uint16 sensor, Uint8 channel, float32 value);
DIESEL_STATUS getSensor(Uint16 sensor, Uint8 channel, float32 &value);
DIESEL_STATUS getOnboardTemp(float32 &temperature);
void cpldLedSwitch(Uint8 led, Uint8 value);
void cpldLedToggle(Uint8 led);
int32 getLastError();


#endif /* DIESEL_DRV_H_ */
