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
	std::string name;
	int status;	// ����������, ������������ Check() - �������� ���������� ��� ���

public:
	virtual int Type();
	virtual int Check();		// �������� ��������� ������ ����������
	int GetStatus();
	//int Init();
	//int Close();
};

/**
 * �� �������������� ��������� ���������� � ������ ������-��������
 */
EC_Device GetDevice(int code);


// �������������� ����������
class EC_Actuator : EC_Device
{
public:
	int Type() {return EC_DTYPE_ACTUATOR;}
	int Write(int val);	// ������ �����, �������������� � ������������ �����
		// ������� ���-�� ��-������� � ������ ����� �� � �����, � � ������� ����.

	// ��������, ��� ������� ���� ���������� ����� ���� ����������� ����������
};

// ������
class EC_Sensor : EC_Device
{
public:
	int Type() {return EC_DTYPE_SENSOR;}
	int Read();	// ������ �����, �������������� � ������������ ����� ���������
		// ��-�������. ��� �� �� �� ����, ������� �� ������ ������
};

/**
 * ������ ���������, ������������ ��� ����� ����� ���������� � �����������.
 */
class EC_DeviceList
{
	std::list<EC_Device> list;

public:
	//EC_DeviceList(std::list<EC_Device> nList) {list = nList;} // �� ����� ������
	//EC_DeviceList(std::list<int> nList);	// ����� � �������� ������ ��� ������ ����������
		// ����-�������������� ���������.
	int Check() {return 0;}

	// ������� ������, �������� ��������� bool-������� ���� �� ������ Check()
};


#endif /* DEVICES_H_ */
