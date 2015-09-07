#include "diesel_drv.h"
#include "emif.h"
#include "spi.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "F2837xD_gpio.h"                // General Purpose I/O Registers
#include "F2837xD_GlobalPrototypes.h"         // Prototypes for global functions within the
                                               // .c files.
#include "F2837xD_Gpio_defines.h"             // Macros used for GPIO support code
//
#include <stddef.h>

#ifdef __cplusplus
}
#endif

#include "../ecu_global_defines.h"

// Индексы адреса датчиков для EMIF
#define ADDR_IDX_INJ_SEL		3
#define ADDR_IDX_SENS_SEL       4
#define ADDR_IDX_HALL_SENS      5
#define ADDR_IDX_IND_SENS       6
#define ADDR_IDX_STEP_1_TIME	7
#define ADDR_IDX_STEP_2_TIME    8
#define ADDR_IDX_DUTY_CYCLE     9
#define ADDR_IDX_INJ_NUM		10
#define ADDR_IDX_INJ_START      11
#define ADDR_IDX_LEDS			12
#define ADDR_IDX_FPGA_SOFT_VER	13
#define ADDR_IDX_WAKEUP_HIGH	14
#define ADDR_IDX_WAKEUP_PERIOD	15

// Сброс выбранного сенсора
#define NO_SENSOR				0x0000

// Настройки скважности
#define DUTY_MIN_PERCENT	4
#define DUTY_MAX_PERCENT	99
#define DUTY_100_PERCENT	100
#define DUTY_STEP			3.125

// Настройки датчика температуры i2c
#define TEMP_SENSOR_I2C_ADDR	0x0048
#define TEMP_SENSOR_I2C_STEP	0.0625
#define TEMP_SENSOR_I2C_EXT_THRESHOLD	150.0
#define TEMP_SENSOR_I2C_EXT_CORRECT		512.0
#define TEMP_SENSOR_I2C_NORM_THRESHOLD	128.0
#define TEMP_SENSOR_I2C_NORM_CORRECT	256.0

void setupGpioMux();
void initF2837Peripheral();
void initOnBoardPeripheral();

static int32 lastError;

//#pragma CODE_SECTION("ramfuncs")
int32 getLastError()
{
	return lastError;
}

#pragma CODE_SECTION("ramfuncs")
void dieselDelay(Uint32 ticks)
{
	// Delay ~ 50ns
	Uint32 i;
	for (i = 0; i < ticks; i++)
		asm(" NOP ");
}

#pragma CODE_SECTION("ramfuncs")
void dieselDrvInit()
{
	setupGpioMux();
	initF2837Peripheral();
	initOnBoardPeripheral();
	
	cpldLedSwitch(LED_RED_1, 0);
	cpldLedSwitch(LED_YELLOW, 0);
	cpldLedSwitch(LED_GREEN, 0);
	cpldLedSwitch(LED_RED_2, 0);
}

//#pragma CODE_SECTION("ramfuncs")
void setupGpioMux()
{
	//EALLOW;
	//__asm(" EALLOW");
	// These functions are found in the F2837xD_Gpio.c file.
	// Setup EMIF
	GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 2);	// ~EM1CS4
	GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 2); // EM1CLK
	GPIO_SetupPinMux(31, GPIO_MUX_CPU1, 2); // ~EM1WE
	GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 2); // ~EM1CS0
	GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 2); // ~EM1RNW
#ifndef IS_DOCKED
	GPIO_SetupPinMux(34, GPIO_MUX_CPU1, 2); // ~EM1CS2
#endif
	GPIO_SetupPinMux(35, GPIO_MUX_CPU1, 2); // ~EM1CS3
	GPIO_SetupPinMux(36, GPIO_MUX_CPU1, 2); // EM1WAIT
	GPIO_SetupPinMux(37, GPIO_MUX_CPU1, 2); // ~EM1OE
	GPIO_SetupPinMux(38, GPIO_MUX_CPU1, 2); // EM1A0
	GPIO_SetupPinMux(39, GPIO_MUX_CPU1, 2); // EM1A1
	GPIO_SetupPinMux(40, GPIO_MUX_CPU1, 2); // EM1A2
	GPIO_SetupPinMux(41, GPIO_MUX_CPU1, 2); // EM1A3
	GPIO_SetupPinMux(44, GPIO_MUX_CPU1, 2); // EM1A4
	GPIO_SetupPinMux(45, GPIO_MUX_CPU1, 2); // EM1A5
	GPIO_SetupPinMux(46, GPIO_MUX_CPU1, 2); // EM1A6
	GPIO_SetupPinMux(47, GPIO_MUX_CPU1, 2); // EM1A7
	GPIO_SetupPinMux(48, GPIO_MUX_CPU1, 2); // EM1A8
	GPIO_SetupPinMux(49, GPIO_MUX_CPU1, 2); // EM1A9
	GPIO_SetupPinMux(69, GPIO_MUX_CPU1, 2); // EM1D15
	GPIO_SetupPinMux(70, GPIO_MUX_CPU1, 2); // EM1D14
	GPIO_SetupPinMux(71, GPIO_MUX_CPU1, 2); // EM1D13
	GPIO_SetupPinMux(72, GPIO_MUX_CPU1, 2); // EM1D12
	GPIO_SetupPinMux(73, GPIO_MUX_CPU1, 2); // EM1D11
	GPIO_SetupPinMux(74, GPIO_MUX_CPU1, 2); // EM1D10
	GPIO_SetupPinMux(75, GPIO_MUX_CPU1, 2); // EM1D9
	GPIO_SetupPinMux(76, GPIO_MUX_CPU1, 2); // EM1D8
	GPIO_SetupPinMux(77, GPIO_MUX_CPU1, 2); // EM1D7
	GPIO_SetupPinMux(78, GPIO_MUX_CPU1, 2); // EM1D6
	GPIO_SetupPinMux(79, GPIO_MUX_CPU1, 2); // EM1D5
	GPIO_SetupPinMux(80, GPIO_MUX_CPU1, 2); // EM1D4
	GPIO_SetupPinMux(81, GPIO_MUX_CPU1, 2); // EM1D3
	GPIO_SetupPinMux(82, GPIO_MUX_CPU1, 2); // EM1D2
	GPIO_SetupPinMux(83, GPIO_MUX_CPU1, 2); // EM1D1
	GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 2); // EM1D0
	GPIO_SetupPinMux(92, GPIO_MUX_CPU1, 3); // EM1BA1
	
	// Setup SPI
	__asm(" EALLOW");
	// Enable internal pull-up for the selected pins
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (SPISTEA)

	// Set qualification for selected pins to asynch only
    GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO57 (SPISTEA)
    __asm(" EDIS");
	
	// Configure SPI-A pins
	GPIO_SetupPinMux(54, GPIO_MUX_CPU1, 1);	// SPISIMOA
	GPIO_SetupPinMux(55, GPIO_MUX_CPU1, 1); // SPISOMIA
	GPIO_SetupPinMux(56, GPIO_MUX_CPU1, 1); // SPICLKA
	GPIO_SetupPinMux(57, GPIO_MUX_CPU1, 1); // SPISTEA
	
	// Setup I2C
	GPIO_SetupPinMux(42, GPIO_MUX_CPU1, 6); // SDAA
	GPIO_SetupPinMux(43, GPIO_MUX_CPU1, 6); // SCLA
	
	// Setup CAN
	GPIO_SetupPinMux(18, GPIO_MUX_CPU1, 3);	// CAN RX A
	GPIO_SetupPinMux(19, GPIO_MUX_CPU1, 3);	// CAN TX A
	GPIO_SetupPinOptions(18, GPIO_INPUT, GPIO_ASYNC);
	GPIO_SetupPinOptions(19, GPIO_OUTPUT, GPIO_PUSHPULL);


	//EDIS;
	//__asm(" EDIS");
}

//#pragma CODE_SECTION("ramfuncs")
void initF2837Peripheral()
{
	emifInit();
	spiInit();
	i2cInit();
}

#pragma CODE_SECTION("ramfuncs")
void cpldLedSwitch(Uint8 led, Uint8 value)
{
	Uint16 ledStatus;
	ledStatus = emifRead(ADDR_IDX_LEDS);
	ledStatus &= (~led) & 0x000F;
	ledStatus |= (value ? led : 0) & 0x000F;
	emifWrite(ADDR_IDX_LEDS, ledStatus);
}

#pragma CODE_SECTION("ramfuncs")
void cpldLedToggle(Uint8 led)
{
	Uint16 ledStatus;
	ledStatus = emifRead(ADDR_IDX_LEDS);
	ledStatus ^= led & 0x000F;	
	emifWrite(ADDR_IDX_LEDS, ledStatus);
}

#pragma CODE_SECTION("ramfuncs")
DIESEL_STATUS setSensor(Uint16 sensor, Uint8 channel, float32 value)
{
	if (sensor != ANALOG_OUTPUT)
	{
		lastError = 0;
		return DIESEL_ERR_ILLEGAL_SENSOR;
	}
		
	float32 Vref = 2.048;
	Uint16 dacCode = (value * 4095) / (2 * Vref);
	
	Uint16 length = 3;
	Uint8 command = 0x03;		// Write to and update DAC Channel n
	Uint8 dataBuf[3];
	dataBuf[0] = (command << 4) | (channel & 0x0F);
	dataBuf[1] = (dacCode/*value*/ >> 4) & 0xFF;
	dataBuf[2] = (dacCode/*value*/ << 4) & 0xF0;
	
	emifWrite(ADDR_IDX_SENS_SEL, sensor);
	dieselDelay(10);
	SPI_STATUS spiRetCode = spiXfer(length, dataBuf, NULL);
	dieselDelay(10000);
	emifWrite(ADDR_IDX_SENS_SEL, NO_SENSOR);
	
	if (spiRetCode)
	{
		lastError = spiRetCode;
		return DIESEL_ERR_SPI_ERROR;
	}
	
	lastError = 0;
	return DIESEL_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
DIESEL_STATUS getSensor(Uint16 sensor, Uint8 channel, float32 &value)
{
	// Ошибка, если аналоговый выход
	if (sensor == ANALOG_OUTPUT)
	{
		lastError = 0;
		return DIESEL_ERR_ILLEGAL_SENSOR;
	}
		
	// Датчики Холла и индуктивный выдаются только чтением EMIF'а
	if ((sensor == HALL_SENSOR) || (sensor == INDUCTIVE_SENSOR))
	{
		value = emifRead(sensor);
		lastError = 0;
		return DIESEL_ERR_NO_ERROR;
	}
	
	// Настройка буферов для подачи команды на АЦП
	Uint16 length = 2;
	Uint8 dataWrBuf[2];
	Uint8 dataRdBuf[2];

	dataWrBuf[0] = (0x01 << 7) |		// Overwrite contents of register
				   (0x06 << 4) |		// Unipolar, referenced to COM = GND
				   (channel << 1) |		// Channel number
				   (0x01);				// Full bandwidth
	dataWrBuf[1] = (0x01 << 5) |		// Internal reference, REF = 4.096, temperature enable
				   (0x00 << 3) | 		// Disable sequencer
				   (0x00 << 2);			// Do not read back contents of configuration

	dataRdBuf[0] = 0;
	dataRdBuf[1] = 0;

	SPI_STATUS spiRetCode;

	// Выбор датчика
	if (sensor == INJECTOR_SENSOR)
	{
		emifWrite(ADDR_IDX_INJ_SEL, channel);
	} else
	{
		emifWrite(ADDR_IDX_SENS_SEL, sensor);
	}
	dieselDelay(10);

	// Если не датчик давления и не форсунка
	if ((sensor != PRESSURE_SENSOR) && (sensor != INJECTOR_SENSOR))
	{
		// Запись команды 2 раза для обеспечения преобразования
		for (int i = 0; i < 2; i ++)
		{
			spiRetCode = spiXfer(length, dataWrBuf, NULL);
			if (spiRetCode)
			{
				emifWrite(ADDR_IDX_SENS_SEL, NO_SENSOR);
				lastError = spiRetCode;
				return DIESEL_ERR_SPI_ERROR;
			}
			dieselDelay(100);	// 5us
		}
		// Запись команды и чтение результата
		spiRetCode = spiXfer(length, dataWrBuf, dataRdBuf);
		if (spiRetCode)
		{
			emifWrite(ADDR_IDX_SENS_SEL, NO_SENSOR);
			lastError = spiRetCode;
			return DIESEL_ERR_SPI_ERROR;
		}
		dieselDelay(100);	// 5us
	} else
	{
		// Считывание данных
		spiRetCode = spiXfer(length, NULL, dataRdBuf);
		dieselDelay(10);
	}
	
	// Сброс выбора датчика
	if (sensor == INJECTOR_SENSOR)
	{
		emifWrite(ADDR_IDX_INJ_SEL, NO_SENSOR);
	} else
	{
		emifWrite(ADDR_IDX_SENS_SEL, NO_SENSOR);
	}

	if (spiRetCode)
	{	
		lastError = spiRetCode;
		return DIESEL_ERR_SPI_ERROR;
	}
	
	Uint16 adcCode;
	// Извлечение полученного кода АЦП
	if ((sensor != PRESSURE_SENSOR)  && (sensor != INJECTOR_SENSOR))
	{
		adcCode = ((dataRdBuf[0] << 8) & 0xFF00) | (dataRdBuf[1] & 0x00FF);
	} else
	{
		adcCode = ((dataRdBuf[0] << 6) & 0x0FC0) | ((dataRdBuf[1] >> 2) & 0x003F);
	}

	const float32 gain = 2 * (100 / 13);

	switch (sensor)
	{
	case PRESSURE_SENSOR:
		// Vref = 5.0, Max ADC code = 4095
		float32 pressureU = adcCode * 5.0 / 4095.0;
		// Формула выведена из граничных значений датчика (линейная зависимость) (min 0.2V=20kPa, max 4.9V=250kPa)
		value = (230.0 * pressureU + 48.0) / 4.7;
		break;

	case INJECTOR_SENSOR:
		value = adcCode * 5.0 / 4095.0;		// Vref = 5V
		break;
		
	case CURRENT_SENSOR:
		value = adcCode * 4.096 / (4095.0 * 200.0);		// I = U / 200Om
		break;
		
	case TEMPERATURE_SENSOR:
	case POTENTIO_SENSOR:
		float32 tempPotU = adcCode * 4.096 / 65535.0;
		value = (tempPotU - 2.5) / gain / 0.00107;			// R = (U-2.5)/1.07mA; 2.5V - напряжение поднято коэффициентом усиления
		break;

	default:
		value = adcCode * 4.096 / 65535.0;	// Vref = 4.096
	}
	
	lastError = 0;
	return DIESEL_ERR_NO_ERROR;
}

//#pragma CODE_SECTION("ramfuncs")
DIESEL_STATUS getOnboardTemp(float32 &temperature)
{
	I2C_STATUS i2cRetCode;
	Uint16 tempCode;
	Uint16 tempAddr = TEMP_SENSOR_I2C_ADDR;
	Uint16 length = 2;
	Uint8 dataRdBuf[2];
	dataRdBuf[0] = 0;
	dataRdBuf[1] = 0;
	i2cRetCode = i2cRead(tempAddr, length, dataRdBuf);
	if (i2cRetCode)
	{
		lastError = i2cRetCode;
		return DIESEL_ERR_I2C_ERROR;
	}
	
	if (dataRdBuf[1] & 0x01)
	{
		// Extended 13-bit mode
		tempCode = ((dataRdBuf[0] << 5) & 0x1FE0) | ((dataRdBuf[1] >> 3) & 0x001F);
		temperature = tempCode * TEMP_SENSOR_I2C_STEP;
		if (temperature > TEMP_SENSOR_I2C_EXT_THRESHOLD)
			temperature -= TEMP_SENSOR_I2C_EXT_CORRECT;
	} else
	{
		// Normal 12-bit mode
		tempCode = ((dataRdBuf[0] << 4) & 0x0FF0) | ((dataRdBuf[1] >> 4) & 0x000F);
		temperature = tempCode * TEMP_SENSOR_I2C_STEP;
		if (temperature > TEMP_SENSOR_I2C_NORM_THRESHOLD)
			temperature -= TEMP_SENSOR_I2C_NORM_CORRECT;
	}
	
	lastError = 0;
	return DIESEL_ERR_NO_ERROR;
}

#pragma CODE_SECTION("ramfuncs")
void setInjector(Uint16 step1TimeUs, Uint16 step2TimeUs, Uint8 dutyCycle1, Uint8 dutyCycle2)
{
	if (dutyCycle1 < DUTY_MIN_PERCENT)
		dutyCycle1 = DUTY_MIN_PERCENT;
	if (dutyCycle1 > DUTY_MAX_PERCENT)
		dutyCycle1 = DUTY_MAX_PERCENT;
		
	if (dutyCycle2 < DUTY_MIN_PERCENT)
		dutyCycle2 = DUTY_MIN_PERCENT;
	if (dutyCycle2 > DUTY_MAX_PERCENT)
		dutyCycle2 = DUTY_MAX_PERCENT;
		
	Uint8 dutyCycle1Code = (DUTY_100_PERCENT - dutyCycle1) / DUTY_STEP;
	Uint8 dutyCycle2Code = (DUTY_100_PERCENT - dutyCycle2) / DUTY_STEP;
	
	Uint16 dutyCycle = ((dutyCycle2Code << 8) & 0xFF00) | (dutyCycle1Code & 0x00FF);
	
	emifWrite(ADDR_IDX_STEP_1_TIME, step1TimeUs);
	emifWrite(ADDR_IDX_STEP_2_TIME, step2TimeUs);
	emifWrite(ADDR_IDX_DUTY_CYCLE, dutyCycle);

	emifWrite(ADDR_IDX_WAKEUP_HIGH, EG::g_wakeup_High);
	emifWrite(ADDR_IDX_WAKEUP_PERIOD, EG::g_wakeup_Period);
}

#pragma CODE_SECTION("ramfuncs")
void startInjector(Uint16 injectorNum)
{
	emifWrite(ADDR_IDX_INJ_NUM, injectorNum);
	emifWrite(ADDR_IDX_INJ_START, 0x0001);
	// сбрасываем таймер и просим считать обратную связь
	EG::tmpi = cylToCode(EG::fdbkCyl);
	if ((injectorNum == EG::tmpi) && EG::manFdbk)
	{
		EG::fdbkTCnt = 0;
		EG::getFdbk = 1;
	}
}

//#pragma CODE_SECTION("ramfuncs")
void initOnBoardPeripheral()
{
	float32 dummyValue;
	getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, dummyValue);
	getSensor(CURRENT_SENSOR, CURR_SENS_CHANNEL_1, dummyValue);
	getSensor(ANALOG_INPUT, ANALOG_IN_CHANNEL_1, dummyValue);
	getSensor(PRESSURE_SENSOR, 0x00, dummyValue);
	getSensor(INJECTOR_SENSOR, INJECTOR_CHANNEL_1, dummyValue);
}
