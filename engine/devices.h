/*
 * devices.h
 *
 *  Created on: 16.09.2014
 *      Author: ����
 */

#ifndef DEVICES_H_
#define DEVICES_H_

//#include <iostream>
#include <string>
#include <list>
#include "ecu_global_defines.h"
#include "drv.h"

// ����������� ��������, ��������������� ��� ������� ����������
#define EC_DTYPE_ACTUATOR 	1
#define EC_DTYPE_SENSOR 	2

#define EC_DSTAT_OK			0	// ���������� �������� ���������
#define EC_DSTAT_FAIL		1	// ���������� �������� �����������
#define EC_DSTAT_MISS		2	// ���������� �����������

/**
 * �������� ������ �������� ������� ����������.
 *
 * ��������, ����� �������� �������������� ��������� � �������
 */
class EC_Device
{
protected:
	//std::string name;
	//int status;	// ����������, ������������ Check() - �������� ���������� ��� ���
	Uint8 _chan;		// ����� ������
	Uint16 _sens;		// ��� �������
	Uint32 _minval;		// ����������� ��������
	Uint32 _maxval;		// ������������ ��������
	float _k;	// ����������� �������� � �����. �������
	Uint32 _val;		// ���������� �������� ���������� �������
	
	// int failedCkeckCnt;	// ���������� ����������� ��������.
		// ��� ���������� ���������� ����� ���������� ��������� �� �������
	
public:
	EC_Device(Uint16 sens, Uint8 chan, Uint32 min, Uint32 max, float k) :
		_sens(sens), _chan(chan), _minval(min), _maxval(max), _k(k) {}
		
	EC_Device(Uint16 sens, Uint8 chan, float min, float max, float k) :
		_sens(sens), _chan(chan), _minval(min/k), _maxval(max/k), _k(k) {}


	//virtual int Type();
	virtual int Check();		// �������� ��������� ������ ����������
	int GetStatus();
	
	virtual float getValue() { return 0; }
	virtual void setValue(float val) {}
	//int Init();
	//int Close();
};

/**
 * �� �������������� ��������� ���������� � ������ ������-��������
 */
//EC_Device GetDevice(int code);


// �������������� ����������
class EC_Actuator : EC_Device
{
public:
	int Type() {return EC_DTYPE_ACTUATOR;}
	void setValue(float val);	// ������ �����, �������������� � ������������ �����
		// ������� ���-�� ��-������� � ������ ����� �� � �����, � � ������� ����.

	// ��������, ��� ������� ���� ���������� ����� ���� ����������� ����������
};

// ������
class EC_Sensor : EC_Device
{
public:
	int Type() {return EC_DTYPE_SENSOR;}
	float getValue();	// ������ �����, �������������� � ������������ ����� ���������
		// ��-�������. ��� �� �� �� ����, ������� �� ������ ������
	
	// ��������� �� ������� ������� ��������? ������ �������, ����� ����� ���� ��������� ��������,
	// ����������� �� ������ �������� � ������ ������ �������.
};

/**
 * ������ ���������, ������������ ��� ����� ����� ���������� � �����������.
 */
class EC_DeviceList
{
	std::map<std::string, EC_Device> list;
		// ������ ���������� ���������� � ��������� ������

public:
	//EC_DeviceList(std::list<EC_Device> nList) {list = nList;} // �� ����� ������
	//EC_DeviceList(std::list<int> nList);	// ����� � �������� ������ ��� ������ ����������
		// ����-�������������� ���������.
	int Check() {return 0;}

	// ������� ������, �������� ��������� bool-������� ���� �� ������ Check()
};


#endif /* DEVICES_H_ */
