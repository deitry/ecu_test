/*
 * engine_vars.cpp
 *
 *  Created on: 11 мая 2015 г.
 *      Author: Например Андрей
 */

#include "engine.h"
#include "math.h"

using namespace EG;

/**
 * Опрос датчика температуры окружающей среды
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tenv()
{
	float32 temp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, temp); // NI: Channel 1
	return 0.9*temp/2;
}

/**
 * Опрос датчика температуры охлаждающей жидкости
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tcool()
{
	return tCool;
}

/**
 * Опрос положения пускового ключа
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Key()
{
	float32 tmp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, tmp); // NI: Channel 1
	return (floor(tmp * 2.76 / 1 + 0.5) * 1 - 100)/10;
}

/**
 * Опрос положения педали
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Pedal()
{
	//pInj = this->devices->getDevice(D_PINJ)->getValue();
	return (floor(this->devices->getDevice(EC_S_D_PEDAL)->getValue() / pedStep + 0.5) * pedStep);
}

