/*
 * devices.h
 *
 *  Created on: 16.09.2014
 *      Author: Дима
 */

#ifndef DEVICES_H_
#define DEVICES_H_

//#include <iostream>
#include <string>
#include <list>
#include "ecu_global_defines.h"
#include "drv.h"

// определения констант, характеризующих тип данного устройства
#define EC_DTYPE_ACTUATOR 	1
#define EC_DTYPE_SENSOR 	2

#define EC_DSTAT_OK			0	// устройство работает нормально
#define EC_DSTAT_FAIL		1	// устройство работает неправильно
#define EC_DSTAT_MISS		2	// устройство отсутствует

/**
 * Описание одного отдельно взятого устройства.
 *
 * Возможно, стоит выделить исполнительные механизмы и датчики
 */
class EC_Device
{
protected:
	//std::string name;
	//int status;	// переменная, выставляемая Check() - работает устройство или как
	Uint8 _chan;		// номер канала
	Uint16 _sens;		// тип датчика
	Uint32 _minval;		// минимальное значение
	Uint32 _maxval;		// максимальное значение
	float _k;	// коэффициент перевода в соотв. единицы
	Uint32 _val;		// сохранённое значение последнего запроса
	
	// int failedCkeckCnt;	// количество проваленных проверок.
		// При превышении некоторого числа устройство считается не рабочим
	
public:
	EC_Device(Uint16 sens, Uint8 chan, Uint32 min, Uint32 max, float k) :
		_sens(sens), _chan(chan), _minval(min), _maxval(max), _k(k) {}
		
	EC_Device(Uint16 sens, Uint8 chan, float min, float max, float k) :
		_sens(sens), _chan(chan), _minval(min/k), _maxval(max/k), _k(k) {}


	//virtual int Type();
	virtual int Check();		// проверка состояния работы устройства
	int GetStatus();
	
	virtual float getValue() { return 0; }
	virtual void setValue(float val) {}
	//int Init();
	//int Close();
};

/**
 * По идентификатору открывает устройство и создаёт объект-описание
 */
//EC_Device GetDevice(int code);


// исполнительное устройство
class EC_Actuator : EC_Device
{
public:
	int Type() {return EC_DTYPE_ACTUATOR;}
	void setValue(float val);	// скорее всего, взаимодействие с устройствами будет
		// сделано как-то по-другому и скорее всего не в общем, а в частном виде.

	// возможно, для каждого типа устройства будет своя собственная реализация
};

// датчик
class EC_Sensor : EC_Device
{
public:
	int Type() {return EC_DTYPE_SENSOR;}
	float getValue();	// скорее всего, взаимодействие с устройствами будет выглядеть
		// по-другому. Как бы то ни было, оставим на всякий случай
	
	// указатель на функцию подмены значения? Именно функция, чтобы можно было вычислять значение,
	// основываясь на других датчиках и режиме работы системы.
};

/**
 * Список устройств, используемых для связи блока управления с реальностью.
 */
class EC_DeviceList
{
	std::map<std::string, EC_Device> list;
		// каждое устройство сязывается с некоторым именем

public:
	//EC_DeviceList(std::list<EC_Device> nList) {list = nList;} // не очень удобно
	//EC_DeviceList(std::list<int> nList);	// здесь в качестве основы для списка передаются
		// коды-идентификаторы устройств.
	int Check() {return 0;}

	// функции опроса, возможно некоторые bool-функции сюда же помимо Check()
};


#endif /* DEVICES_H_ */
