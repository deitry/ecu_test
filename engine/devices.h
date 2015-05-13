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
#include <map>
#include "../ecu_global_defines.h"
#include "../drv/diesel_drv.h"

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
	float32 _min;		// ����������� ��������
	float32 _max;		// ������������ ��������
	float _k;	// ����������� �������� � �����. �������
	float32 _val;		// ����������� �������� ���������� �������
	
	// int failedCkeckCnt;	// ���������� ����������� ��������.
		// ��� ���������� ���������� ����� ���������� ��������� �� �������
	
public:
	EC_Device(Uint16 sens, Uint8 chan, float32 min, float32 max, float k) :
		_sens(sens), _chan(chan), _min(min/k), _max(max/k), _k(k), _val(0) {}


	//virtual int Type();
	int Check();		// �������� ��������� ������ ����������
	int GetStatus();
	
	virtual float getValue() { return 0; }
	virtual void setValue(float val) {}

	// "�������" ������ � ������� ��� ���������.
	// � ������ �������� ������ � �������� �������� ������������ ��������� ��������� - _val,
	// ��� ���� ���������� ���� � ���, ��� ��� ������ �� ������� ������ � �� ����� ���������
	// ��������� Check (���� ���������� ������ �� ������ �������� ������, �� �� ��������� ���� �����)
	// ����� �������� _val. ���������� � ������� - ���� �������� ������ �� ������� ��������� ��������,
	// �� ��� ��������������� � ������������ _val, � �� ���������� ��� ��������� ��� ������ ��
	// ��������� Check() ��� setValue()
	//virtual float getValueFast() { return 0; }
	//virtual void setValueFast(float val) {}

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
	EC_Actuator(Uint16 sens, Uint8 chan, float32 min, float32 max, float k)
		: EC_Device(sens, chan, min, max, k) {};

	int Type() {return EC_DTYPE_ACTUATOR;}
	void setValue(float val);	// ������ �����, �������������� � ������������ �����
		// ������� ���-�� ��-������� � ������ ����� �� � �����, � � ������� ����.

	// ��������, ��� ������� ���� ���������� ����� ���� ����������� ����������
};

// ������
// TODO : �������� ����� FilteredSensor ��� �������� ������ �����-���� ������ ��������, ��������, ��� ���������.
// ��� ���� ������� ������������ ������ ����� ����������� ������������ �����������.
class EC_Sensor : EC_Device
{
private:
	int _filter;
public:

	static const int F_PREVIOUS = 1;	// ��� ������ �� ����������� � �������� �������� �������� ������ ����������

	EC_Sensor(Uint16 sens, Uint8 chan, float32 min, float32 max, float k, int filter)
		: EC_Device(sens, chan, min, max, k), _filter(filter) {};

	int Check();
	int Type() {return EC_DTYPE_SENSOR;}
	float getValue();	// ������ �����, �������������� � ������������ ����� ���������
		// ��-�������. ��� �� �� �� ����, ������� �� ������ ������
	
	// ��������� �� ������� ������� ��������? ������ �������, ����� ����� ���� ��������� ��������,
	// ����������� �� ������ �������� � ������ ������ �������.
};

/**
 * ������ ��������������� ������������ � ������� ���������.
 */
#define D_PEDAL		0x01	// ������
#define D_KEY		0x02	// ����
#define D_PK		0x03	// �������� �������
#define D_PINJ		0x04	// �������� �������
#define D_TV		0x05

/**
 * ������ ���������, ������������ ��� ����� ����� ���������� � �����������.
 */
class EC_DeviceList
{
	std::map<int, EC_Device*> list; // ������ ���������� ���������� � ��������� ������

public:
	EC_DeviceList() {}
	~EC_DeviceList();
	int Check() { return 0; }
	void addDevice(int id, EC_Device* device) { list[id] = device; }
	EC_Device* getDevice(int id) { return list[id]; }

	// ������� ������, �������� ��������� bool-������� ���� �� ������ Check()
};


#endif /* DEVICES_H_ */
