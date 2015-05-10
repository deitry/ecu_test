/*
 * engine.h
 *
 *  Created on: 15.09.2014
 *      Author: Дима
 */

#ifndef ENGINE_H_
#define ENGINE_H_

//#include "ecu_global_defines.h"
#include "devices.h"
#include "hardware.h"
#include "../can_par_id.h"

// список режимов
#define EC_MAX_MODE 10
enum eng_mod { 	EC_Start = 1,		// пуск
				EC_Steady,			// холостой ход
				EC_Transient,		// педаль-уставка
				EC_Automotive,		// педаль-подача
				EC_Econom,			// экономичность
				EC_MaxDyn,			// максимальная динамика
				EC_Finish,			// останов
				EC_Failure,			// ошибка
				EC_Crash,			// хана
				EC_End};			// конец

// вспомогательные функции.
void sendCanMsgX(CAN_DATA* data);
void sendCanMsg(int id, Uint32* val);
void sendCanMsg(int id, float* val);

float timeToAngle(float time);
float angleToTime(float angle);


// Описывает двигатель
class EC_Engine
{
public:
	EC_Engine();		// базовый конструктор

	int Run();

protected:
	// Список базовых режимов. Они оформлены как самостоятельные процедуры.
	// Каждый режим определяет некоторую _однократную_ последовательность действий.
	// Цикл должен быть только один - глобальный (Run())
	int Initialise();	// инициализация системы управления - нужна ли отдельно?
	int Start();		// пуск
	int Finish();		// завершение работы
	int Steady();		// стационарный режим - холостой ход
	int Transient();	// переход на другой режим
	int Automotive();	// переход на другой режим
	int Failure();		// аварийный режим
	int Finalise();		// финализация системы управления - нужно ли отдельно?

	//int SysCheck();	// проверка состояния системы в целом и отдельных элементов
	int ControlCheck();	// проверка состояния положения органов управления
	int ModeCalc();		// перерасчёт режима работы

	int Monitoring(void);	// недалеко ушла от SysCheck(), но выполняется регулярно, по таймеру.

	eng_mod mode;		// режим

protected:

	// - - - изменение режима
	static Uint16 QCtoUS(float qc);
	void setInjPhi(void);

	void setQCTrans(void);
	void setQCAuto(void);
	void setQCSteady(void);
	void setQCrelay(void);

	// - - - запросы к переменным выполнены в виде функций - запросы к драйверам низкого уровня
	float WD();		// частота вращения
	float Tenv();	// температура наружного воздуха
	float Tcool();	// температура охлаждающей жидкости
	float Pedal();	// положение педали: 0-100
	int Key();		// положение пускового ключа

	// - - - инкапсулировать отдельным блоком различные константы?
	float TenvMin;	// минимально допустимое значение Т окр ср, при которой не нужен прогрев
	float TcoolMin;	// допустимое значение Тохл, при которой не нужен нагрев
	float tCool;

	// Пригодится "карта" устройств, на которой можно было
	// бы отмечать состояние датчиков и в зависимости от него корректировать
	// программу управления. Можно и нужно оформить в виде отдельного класса, который
	// будет отвечать за взаимодействие блока с окружающим миром.
	EC_DeviceList* devices;

	/**
	 * Все доступные методы по взаимодействию с устройством
	 */
	EC_Hardware* hard;


public:
	int sendCanMsg(PAR_ID_BYTES id);
	void recieveCanMsg(tCANMsgObject* canSendMessage);
};



#endif /* ENGINE_H_ */
