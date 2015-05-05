/*
 * devices.cpp
 *
 *  Created on: 19.09.2014
 *      Author: ƒима
 */

#include "devices.h"

/**
 * ѕолучение статуса устройства: есть/отсутствует/не работает
 */
int EC_Device::GetStatus()
{
	return 0;
}

float EC_Sensor::getValue()
{
	getSensor(_chan, _sens, _val);
	return _val * _k;
}

int EC_Sensor::Check()
{
	if ((_minval <= _val) && (_val <= _maxval))
	{
		return 0;
	}
	else
		return 1;
}

void EC_Actuator::setValue(float val)
{
	_val = val / _k;
	
	if (_minval > _val)
	{
		// ограничение снизу
		setSensor(_chan, _sens, _minval);
	}
	else if (_val > _maxval)
	{
		// ограничение сверху
		setSensor(_chan, _sens, _maxval);
	}
	else
	{
		// нормальна€ работа
		setSensor(_chan, _sens, _val);
	}
}
