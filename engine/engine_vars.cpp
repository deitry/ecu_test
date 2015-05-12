/*
 * engine_vars.cpp
 *
 *  Created on: 11 ��� 2015 �.
 *      Author: �������� ������
 */

#include "engine.h"
#include "math.h"

using namespace EG;

/**
 * ����� ������� ����������� ���������� �����
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tenv()
{
	float32 temp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, temp); // NI: Channel 1
	return 0.9*temp/2;
}

/**
 * ����� ������� ����������� ����������� ��������
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tcool()
{
	return tCool;
}

/**
 * ����� ��������� ��������� �����
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Key()
{
	float32 tmp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, tmp); // NI: Channel 1
	return (floor(tmp * 2.76 / 1 + 0.5) * 1 - 100)/10;
}

/**
 * ����� ��������� ������
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Pedal()
{
	//float32 pedValue1;
	//DIESEL_STATUS getTempStatus1 = getSensor(sens, chan, pedValue1); // NI: Channel 1  // TEMP_SENS_CHANNEL_2
	//return floor(pedValue1 * 2.77 * 5 / HMLTP / pedStep + 0.5) * pedStep; //
	pInj = this->devices->getDevice(D_PINJ)->getValue();
	return (floor(this->devices->getDevice(D_PEDAL)->getValue() / pedStep + 0.5) * pedStep);
}

