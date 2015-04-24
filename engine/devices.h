/*
 * devices.h
 *
 *  Created on: 16.09.2014
 *      Author: ƒима
 */

#ifndef DEVICES_H_
#define DEVICES_H_

//#include <iostream>
#include <string>
#include <list>

// определени€ констант, характеризующих тип данного устройства
#define EC_DTYPE_ACTUATOR 	1
#define EC_DTYPE_SENSOR 	2

#define EC_DSTAT_OK			0	// устройство работает нормально
#define EC_DSTAT_FAIL		1	// устройство работает неправильно
#define EC_DSTAT_MISS		2	// устройство отсутствует

/**
 * ќписание одного отдельно вз€того устройства.
 *
 * ¬озможно, стоит выделить исполнительные механизмы и датчики
 */
class EC_Device
{
	std::string name;
	int status;	// переменна€, выставл€ема€ Check() - работает устройство или как

public:
	virtual int Type();
	virtual int Check();		// проверка состо€ни€ работы устройства
	int GetStatus();
	//int Init();
	//int Close();
};

/**
 * ѕо идентификатору открывает устройство и создаЄт объект-описание
 */
EC_Device GetDevice(int code);


// исполнительное устройство
class EC_Actuator : EC_Device
{
public:
	int Type() {return EC_DTYPE_ACTUATOR;}
	int Write(int val);	// скорее всего, взаимодействие с устройствами будет
		// сделано как-то по-другому и скорее всего не в общем, а в частном виде.

	// возможно, дл€ каждого типа устройства будет сво€ собственна€ реализаци€
};

// датчик
class EC_Sensor : EC_Device
{
public:
	int Type() {return EC_DTYPE_SENSOR;}
	int Read();	// скорее всего, взаимодействие с устройствами будет выгл€деть
		// по-другому.  ак бы то ни было, оставим на вс€кий случай
};

/**
 * —писок устройств, используемых дл€ св€зи блока управлени€ с реальностью.
 */
class EC_DeviceList
{
	std::list<EC_Device> list;

public:
	//EC_DeviceList(std::list<EC_Device> nList) {list = nList;} // не очень удобно
	//EC_DeviceList(std::list<int> nList);	// здесь в качестве основы дл€ списка передаютс€
		// коды-идентификаторы устройств.
	int Check() {return 0;}

	// функции опроса, возможно некоторые bool-функции сюда же помимо Check()
};


#endif /* DEVICES_H_ */
