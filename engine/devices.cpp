/*
 * devices.cpp
 *
 *  Created on: 19.09.2014
 *      Author: ����
 */

#include "devices.h"


/**
 * ��������� ������� ����������: ����/�����������/�� ��������
 */
int EC_Device::GetStatus()
{
	return 0;
}

float EC_Sensor::getValue()
{
	float32 val;
	getSensor(_sens, _chan, val);
	switch (_filter)
	{
	case F_PREVIOUS:
		if ((val < _max) && (val > _min))
			_val = val*0.2 + _val*0.8;
		break;
	}
	return _val * _k + _b;
}

int EC_Sensor::Check()
{
	if ((_min <= _val) && (_val <= _max))
	{
		return 0;
	}
	else
		return 1;
}

void EC_Actuator::setValue(float val)
{
	// _b ����� �� �����������
	_val = val / _k;
	
	if (_min > _val)
	{
		// ����������� �����
		setSensor(_sens, _chan, _min);
	}
	else if (_val > _max)
	{
		// ����������� ������
		setSensor(_sens, _chan, _max);
	}
	else
	{
		// ���������� ������
		setSensor(_sens, _chan, _val);
	}
}

EC_DeviceList::~EC_DeviceList()
{

}
