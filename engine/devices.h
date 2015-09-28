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
#include <map>
#include "../ecu_global_defines.h"
#include "../drv/diesel_drv.h"

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
	float32 _min;		// минимальное значение
	float32 _max;		// максимальное значение
	float _k;			// коэффициент перевода в соотв. единицы
	float _b;			// ещё коэффициент для y = kx + b
	float32 _val;		// сохранённое значение последнего запроса
	
	// int failedCkeckCnt;	// количество проваленных проверок.
		// При превышении некоторого числа устройство считается не рабочим
	
public:
	EC_Device(Uint16 sens, Uint8 chan, float32 min, float32 max, float k, float b)
		: _sens(sens), _chan(chan), _min((min-b)/k), _max((max-b)/k), _k(k), _b(b), _val(0) {}


	//virtual int Type();
	int Check();		// проверка состояния работы устройства
	int GetStatus();
	
	virtual float getValue() { return 0; }
	virtual void setValue(float val) {}

	// "быстрое" чтение с датчика или установка.
	// В случае быстрого чтения в качестве значения возвращается последнее считанное - _val,
	// при этом возводится флаг о том, что был запрос на быстрое чтение и во время следующей
	// процедуры Check (если нормальный запрос на чтение случится раньше, то всё произойдёт само собой)
	// нужно обновить _val. Аналогично с записью - если поступил запрос на быструю установку значения,
	// то оно устанавливается в кэшированное _val, а на устройство оно поступает уже вместе со
	// следующим Check() или setValue()
	//virtual float getValueFast() { return 0; }
	//virtual void setValueFast(float val) {}

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
	EC_Actuator(Uint16 sens, Uint8 chan, float32 min, float32 max, float k, float b)
		: EC_Device(sens, chan, min, max, k, b) {};

	int Type() {return EC_DTYPE_ACTUATOR;}
	void setValue(float val);	// скорее всего, взаимодействие с устройствами будет
		// сделано как-то по-другому и скорее всего не в общем, а в частном виде.

	// возможно, для каждого типа устройства будет своя собственная реализация
};

// датчик
// TODO : отделить класс FilteredSensor или добавить фильтр каким-либо другим способом, например, как декоратор.
// При этом функция фильтрования должна иметь возможность подключаться динамически.
class EC_Sensor : EC_Device
{
private:
	int _filter;
public:

	static const int F_PREVIOUS = 1;	// при выходе за ограничения в качестве текущего значения берётся предыдущее

	EC_Sensor(Uint16 sens, Uint8 chan, float32 min, float32 max, float k, float b, int filter)
		: EC_Device(sens, chan, min, max, k, b), _filter(filter) {};

	int Check();
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
	std::map<int, EC_Device*> list; // каждое устройство сязывается с некоторым именем

public:
	EC_DeviceList() {}
	~EC_DeviceList();
	int Check() { return 0; }
	void addDevice(int id, EC_Device* device) { list[id] = device; }
	EC_Device* getDevice(int id) { return list[id]; }

	// функции опроса, возможно некоторые bool-функции сюда же помимо Check()
};


#endif /* DEVICES_H_ */
