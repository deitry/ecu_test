/*
 * ecu_global_defines.h
 *
 *  Created on: 11.02.2015
 *      Author: Дима
 */

#ifndef ECU_GLOBAL_DEFINES_H_
#define ECU_GLOBAL_DEFINES_H_

typedef bool _Bool;

#include "F2837xD_Cla_typedefs.h"
#include "data/field.h"
#include "drv/diesel_drv.h"
#include "drv/can_if.h"
#include "can_par_id.h"
//#include "engine/engine.h"

#define HALL_CRANKSHAFT		0				// какой вал используется. 0 - распределительный, 1 - коленчатый (частый)
#define HMLTP	(HALL_CRANKSHAFT ? 1 : 2)	// Hall MuLTiPlier
#define HALL_POLARITY		0				// 1 - прерывания по передним фронтам, 0 - по задним фронтам

#define DIESEL_N_CYL	1			// количество цилиндров
#define DIESEL_D_ZONE	0.5			// зона нечувствительности регулятора - лучше сделать переменной
#define DIESEL_Z_CUT	1			// количество вырезанных зубов
#define DIESEL_Z_ALL	45			// общее количество зубов с учётом вырезанных
#define DIESEL_Z_PHI	(360/DIESEL_Z_ALL)		// угол от одного зуба до другого
#define DIESEL_Z_MAX	(DIESEL_Z_ALL-DIESEL_Z_CUT) 	// количество присутствующих зубов
#define DIESEL_PHI_MAX	(720/HMLTP)		// угол поворота за цикл (два оборота)
#define DIESEL_T_SETUP	3000		// время ожидания (в циклах) после использования setInjector
#define DPM				DIESEL_PHI_MAX	// сокращение

#define FDBK_BUF		50				// размер буфера для обратной связи
#define FDBK_C1			20
#define FDBK_C2			10
#define FDBK_C3			(FDBK_BUF-FDBK_C1-FDBK_C2)
#define PARIDMAX 		41				// количество параметров для вывода на CAN
#define SENSCNT			8				// количество каналов для считывания произвольных датчиков

#define	 	TIMER_FREQ		200		//Specified in MHz
#define	 	TIM1_DIV		100		// делитель, определяющий частоту прерываний.
									// 100 = 100 мкс, 1 = 1мкс
#define	 	TIM2_DIV		10		// делитель, определяющий частоту прерываний.
#define		TIMER_PER		1		//Specified in microseconds
#define		TIMER1_PER		4		//Specified in microseconds
#define 	S2US			1e6		// секунды в микросекунды
#define 	PI				(3.1416)


union CAN_DATA_VAL {
	double f;
	unsigned long int i;
};

struct CAN_DATA_FIELDS {                      // bits description
    Uint8 PARID;
    Uint8 R1;
    Uint8 R2;
    Uint8 R3;
    CAN_DATA_VAL val;
};

union CAN_DATA {
    Uint16 all[6];
    CAN_DATA_FIELDS f;
};

/**
 * Пространство имён для всех глобальных переменных
 *
 * EG = ECU_GLOBAL
 */
namespace EG
{
	extern float injPhi[DIESEL_N_CYL];		// угол, обозначающий момент впрыска
	extern int injZ[DIESEL_N_CYL];			// зуб, начиная с которого
	extern int dTime[DIESEL_N_CYL];			// время в мкс, которое отсчитывается от зуба
	extern Uint32 injCnt[DIESEL_N_CYL];		// счётчик, отсчитывающий с частотой прерываний таймера время для впрыска
	extern Uint8 injSw[DIESEL_N_CYL];		// флаг, обозначающий необходимость впрыска в соответствующий цилиндр
	extern Uint32 injN[DIESEL_N_CYL];		// число прошедших впрысков
	//extern bool injection;

	extern float err, errI, errD;			// составляющие ошибки для расчёта подачи по ПИДу
	extern float kP, kI, kD;				// коэффициенты ПИДа
	extern float nR;						// текущая частота, об/мин
	extern float nU;						// уставка частоты, об/мин
	extern float omegaR;					// текущая частота, рад/с
	extern float QC, QCmin, QCmax;			// подача, кг/цикл, нижнее и верхнее ограничение
	extern float QCadop, alphaDop;			// для ограничения по альфе
	extern volatile Uint8 g_duty1;			// скважность форсирующего сигнала
	extern volatile Uint8 g_duty2;			// скважность удерживающего сигнала
	extern volatile Uint16 g_step1Us;		// продолжительность форсирующего сигнала, мкс
	extern volatile Uint16 g_step2Us;		// продолжительность удерживающего сигнала, мкс
	extern volatile Uint16 g_step3Us;		// промежуток времени для просмотра производной
											// по току при переходе с форсирующего на удерживающий

	extern float muN;						// коэффициент темпа набора частоты вращения
											// (по факту - темп изменения уставки)


	extern float mag;						// волшебный поправочный коэффициент
	extern int magi;
	extern float kQc;						// переводной коэффициент граммы-градусы-обороты

	extern int manQC;						// ручной режим задания подачи
	extern int manQCt;						// задание подачи в мкс
	extern int manQCRelay;						// задание подачи в мкс
	extern int manPhi;						// ручной режим задания момента начала впрыска в градусах
	extern int manOUVT;						// ручной режим задания угла опережения впрыска топлива
											// (базовый угол рассчитывается автоматически)
	extern float injOuvt;					// угол опережения впрыска топлива для задания в ручном режиме
	extern int manMode;						// ручной режим задания режима работы блока
											// (сейчас - по сигналу на резистивном датчике)
	extern int manPed;						// ручной режим задания значения педали
	extern int manInj;						// ручной режим впрыска
	extern int manN;						// период подачи импульсов на форсунку в ручном режиме в мкс
											// (если ноль - работает однократный впрыск)
	extern int injOnce;						// флаг для однократного впрыска
											// (не сбрасывается, если manN > 0)
	extern int injCyl;						// цилиндр, в который осуществляется впрыск в ручном режиме
	extern int manCnt;						// счётчик для ручного режима впрыска
	extern int manAngle;					// включение режима задания прожолжительности впрыска в градусах
											// (для работы необходимо включить manQC)
	extern float injAngle;					// продолжительность впрыска в градусах

	extern int canSend;						// отправка параметров по CAN
	extern int canTime;						// отправка временных меток
	extern int manFdbk;						// вывод обратной связи по току
	extern int manDur;						// вывод обратной связи по току
	extern int manLed;						// мигание светодиодами
	extern int manLed1;						// мигание светодиодами
	extern int manLed2;						// мигание светодиодами
	extern int manSens;						// оцифровка датчиков (Pk, Tv...)
	extern int pedStep;						// шаг изменения педали - округляется до этих значений

	extern float pedal;						// положение педали (? с учётом перевода в частоту вращения?)
	extern int finTime;						// время ожидания до выхода из режима EC_Finish после достижения 400 об/мин

	extern float Pk;						// давление наддува (сейчас не используется)
	extern float Tv;						// температура воздуха на входе

	extern float anVMT;

	// переменные для опроса датчиков
	extern float32 val;						// значение
	extern Uint16 sens;						// тип датчика
	extern Uint8 chan;						// канал

	extern float delta_time;
	extern float int_time;
	extern float int_time1;
	extern Uint32 ttim;
	extern float prog_time;

	extern int inOut;						// переключатель между опросами датчиков и выводом значений на испольнительные устройства
	extern float32 valX[SENSCNT];
	extern Uint16 sensX[SENSCNT];
	extern Uint8 chanX[SENSCNT];

	extern float32 valP;	// Pk
	extern Uint16 sensP;
	extern Uint8 chanP;
	extern float Pk_koeff;
	extern float32 valT;	// Tv
	extern Uint16 sensT;	// температура воздуха на впуске
	extern Uint8 chanT;
	extern float Tv_koeff;

	extern float nU0;						// исходная уставка (? сделать частотой холостого хода?)

	extern float dZone;						// зона нечувствительности ПИДа
	extern float dOmega;					// приращение частоты вращения
	//extern float errDMax;					// (не исп.) верхнее ограничение производной от ошибки
	extern float errRelayMax;				// ограничение изменения ошибки (в отрицательную сторону - рост частоты вращения), после к-го мы сбрасываем подачу
	extern float QCprev;					// (не исп.) предыдущее значение подачи

	extern tCANMsgObject canReceiveMessage;	// объект, определяющий приходящее по CAN сообщение
	extern CAN_DATA canTransmitMessage;	// объект, определяющий приходящее по CAN сообщение
	extern Uint8 canReceiveData[8];			// данные в приходящем сообщении

	extern float32 fdbkBuf[FDBK_BUF];		// буфер для обратной связи с форсунками
	extern int fdbkTBuf[FDBK_BUF];		// буфер для обратной связи с форсунками

	extern PAR_ID_BYTES canTransmitId[PARIDMAX];			// TODO : переделать на циклический список

	extern CAN_DATA can_data;
	extern int cntCanTransmit;
	extern int cntCanTransmit2;
	extern PAR_ID_BYTES curCanId;
	extern int canLock;
	extern int canLockM;
	extern int fdbkLock;		// блокирование записи в буфер до полной отправки
	extern int fdbkAll;			// блокирование отправки буфера до новой записи
	extern Uint8 fdbkChan;

	// параметры для определения временной метки
	extern PAR_ID_BYTES tPid;
	extern Uint8 zCnt;		// счётчик зубьев
	extern Uint16 nCnt;		// счётчик оборотов
	extern Uint32 timeCnt;		// счётчик времени

	extern int tmpi;
	extern PAR_ID_BYTES tparid;

	extern int progCnt;
	extern int fdbkTCnt;
	extern int getFdbk;		// снимаем обратную связь
	//extern int

	extern Uint16 n1;
	extern Uint16 n2;
	extern Uint16 n3;
	extern Uint16 d1;
	extern Uint16 d2;
	extern Uint16 d3;
}

/**
 * Пространство имён, объединяющее таблицы с данными
 *
 * EGD = ECU_GLOBAL_DATA
 */
namespace EGD
{
	extern const double OUVTdata[15][15];	// углы опережения впрыска топлива в зав-ти от частоты и подачи
	extern const double SpCharData[7];		// x0400 d100 max7
	extern const double NDSteadyData[3];	// частота холостого хода в зависимости от Тохл
	extern const double QCStartData[3];		// пусковое значение подачи в зависимости от Тохл

	extern FMField2* OUVT;					// угол опережения впрыска топлива
	extern FMField1* SpChar;				// скоростная характеристика
											// (макс подача в зависимости от частоты)
	extern FMField1* NDSteady;
	extern FMField1* QCStart;
}


#endif /* ECU_GLOBAL_DEFINES_H_ */



