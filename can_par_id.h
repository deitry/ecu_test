/*
 * can_par_id.h
 *
 * Содержит определения идентификаторов для передачи параметров через CAN.
 *
 * Таблицы передаются следующим образом:
 * - основной идентификатор обозначает вид таблицы,
 * - второстепенные - индекс текущего значения в этой таблице
 * (максимально может быть до трёх индексов, т.е. мы можем передавать трёхмерные таблицы)
 *
 * Таким же образом можно выделять группы параметров.
 * В пределах одной группы может быть выделено 256 идентификаторов.
 *
 *  Created on: 18.03.2015
 *      Author: Дима
 */

#ifndef CAN_PAR_ID_H_
#define CAN_PAR_ID_H_

struct PAR_ID_BYTES
{
	Uint8 P:8;
	Uint8 S:8;
	Uint8 S2:8;
	Uint8 S3:8;
};

// элемент списка для посылки по CAN
// Обойдёмся без контейнера, потому что не нужен богатый функционал, а место экономить надо
class CANListElement
{
public:
	PAR_ID_BYTES current;
	CANListElement* next;
	//bool single;	// один раз отправили, можно из списка удалить

	static CANListElement* first;	// всегда храним ссылку на первый элемент, чтобы проще было искать концы, если что

public:
	CANListElement(PAR_ID_BYTES id, CANListElement* parent = 0) //,isSingle= false
	{
		current = id;
		next = 0;
		//single = isSingle;

		if (!parent)
		{
			parent = Last();
		}

		if (!parent)
		{
			first = this;
		}
		else
		{
			parent->next = this;
		}
	}

	static CANListElement* Last()
	{
		CANListElement* last = first;
		if (!last) return 0;

		while (last->next != 0)
		{
			last = last->next;
		}
		return last;
	}

	static void Clear()
	{
		while (first != 0)
		{
			//CANListElement* nxt = first->next;
			//delete first;
			//first = nxt;
			// интересно, какой вариант лучше?
			// во втором варианте first всегда соответствует чему-то адекватному,
			// в первом он некоторое время недоступен.
			// во втором ничего лишнего удаляться не будет?..
			CANListElement* cur = first;
			first = first->next;
			delete cur;
		}
	}
};

/**
 * Запрос на получение параметра. Формат данных в таком случае:
 * (0xFF) - (P) - (S) - (SS) - (SSS)
 */
#define EC_PREQ		0xFB		// одиночный запрос параметра
#define EC_PQUE		0xFA		// добавление параметра в список постоянной передачи
#define EC_PCLR		0xF9		// обнуление списка постоянной передачи


#define EC_TIME		0xFE		// метка времени на момент прихода прерывания от зуба
#define EC_TINJ		0xFD		// метка времени на момент впрыска
#define EC_TPROG	0xFC		// метка времени на новом цикле программы
#define EC_THALL	0xF8		// метка времени на "нулевом" зубе

#define EC_P0		0x00

#define EC_BAD		0xFF
/**
 * Расшифровка сокращений:
 * - EC = Electronic Control
 * - P - Parameter - параметр
 * - G - группа параметров
 * - T - Table - таблица. Также используется для группирования
 * параметров для каждого из цилиндров
 * - S - Sub-parameter / Sub-index 1
 * индексация подпараметров начинается с 1,
 * подиндексов в таблицах и группах с 0
 * - SS - Sub-index 2
 */

#define EC_P_MODE		0x01			// режим
#define EC_G_N			0x02			// группа параметров: частота вращения
#define 	EC_S_NR				0x00	// текущая частота
#define 	EC_S_NU				0x01	// уставка
#define 	EC_S_OMEGA			0x02	// частота в рад/с
#define 	EC_S_DTIME			0x03	// время между зубьями (датчик Холла)

#define EC_G_QC			0x03			// группа параметров: подача
#define 	EC_S_QC_T			0x00	// продолжительность удерживающего импульса
#define 	EC_S_QC_AN			0x01	// продолжительность в углах
#define 	EC_S_QC				0x02	// подача кг/цикл
#define		EC_S_ADOP			0x03	// ограничение по альфе
#define		EC_S_QC_ADOP		0x04	// используется для передачи QCmax // ограничение подачи по альфе
#define 	EC_S_QC_MAX			0x05	// максимальное значение подачи
#define 	EC_S_QC_MIN			0x06	// минимальное значение подачи
#define		EC_S_KQC			0x07	// переводной коэффициент кг/л -> мкс
#define EC_P_NCYL		0x04			// количество цилиндров
	// подиндекс (i-1) будет использоваться для указания канала, соответствующего данному цилиндру
#define EC_P_PED		0x05			// значение педали
#define 	EC_S_PEDST			0x01	// шаг изменения значения педали
#define EC_P_VMT		0x06			// угол смещения ВМТ относительно пропущенных зубьев

#define EC_G_INJ		0x10			// параметры впрыска
#define 	EC_S_INJT1			0x00	// продолжительность форсирующего
#define 	EC_S_INJT2			0x01	// продолжительность удерживающего
#define 	EC_S_INJD1			0x02	// скважность форсирующего
#define 	EC_S_INJD2			0x03	// скважность удерживающего

// подиндекс - номер цилиндра 0..NCYL
#define EC_T_INJPHI		0x11			// угол поворота - момент впрыска
#define EC_T_INJZ		0x12			// номер зуба, от которого отсчитывается впрыск
#define EC_T_INJN		0x13			// количество впрысков в соответствующий цилиндр
#define EC_T_INJT		0x14			// задержка в мкс от указанного в InjZ зуба
#define EC_T_INJCNT		0x15			// количество отсчётов таймера с момента установки флага на впрыск


#define EC_P_M_MODE		0x21			// ручной режим работы блока
#define EC_P_M_QC		0x22			// ручная установка подачи
#define 	EC_S_M_AN			0x01	// ручная установка подачи в углах
#define 	EC_S_M_IAN			0x02	// угол для установки в качестве подачи
#define		EC_S_M_QCT			0x03	// ручная установка подачи в мкс
#define EC_P_M_INJ		0x23			// ручной режим впрыска
#define 	EC_S_M_IONCE		0x01	// единичный впрыск или запуск
#define 	EC_S_M_IN			0x02	// частота впрысков
#define 	EC_S_M_INJCYL		0x03	// цилиндр, в который осуществляется впрыск
#define 	EC_S_FDBKCYL		0x04	// цилиндр, с которого будет считываться обратная связь
#define EC_P_M_PED		0x24			// ручная установка значения педали
#define EC_P_M_CANS		0x25			// передача сообщений CAN
#define EC_S_M_CANST			0x01	// передача меток времени
#define EC_P_M_UOVT		0x26			// ручной режим установки УОВТ
#define 	EC_S_M_UOVT			0x01	// угол опережения впрыска топлива
#define EC_P_M_FDBK		0x27			// режим снятия обратной связи
	// подиндекс указывает номер точки в массиве обратной связи
#define EC_P_M_LED		0x28			// включение светодиодов
#define EC_P_M_SENS		0x29			// данные от оцифровки датчиков
#define 	EC_S_D_PEDAL		0x01	// педаль
#define 	EC_S_D_KEY			0x02	// ключ
#define 	EC_S_D_PK			0x03	// давление наддува
#define 	EC_S_D_PINJ			0x04	// давление впрыска
#define 	EC_S_D_TV			0x05	// температура воздуха

#define EC_T_UOVT		0x31			// таблица УОВТ
#define EC_T_SPCHAR		0x32			// таблица - скоростная характеристика
#define EC_T_NSTEADY	0x33			// таблица - зависимость частоты ХХ от ?
#define EC_T_QSTART		0x34			// табоица - зависимость стартовой подачи от ?

// ПИД
#define EC_P_KP			0x41			// Кп
#define EC_P_KI			0x42			// Ки
#define EC_P_KD			0x43			// Кд
#define EC_P_ERR		0x44			// еррП * Кп
#define EC_P_ERRI		0x45			// еррИ * Ки
#define EC_P_ERRD		0x46			// еррД * Кд

#define EC_P_MUN		0x47			// коэффициент набора частоты вращения
#define EC_P_DZONE		0x48			// зона нечувствительности

#define EC_P_PTIME		0x51			// время работы программы
#define EC_P_ITIME		0x52			// время XINT1
#define EC_P_ITIME1		0x53			// время TIM1

// Группа параметров, отвечающих за чтение произвольных каналов
#define EC_G_CHAN		0x61
#define 	EC_T_VAL			0x00	// значение
#define 	EC_T_CHAN			0x01	// канал
#define 	EC_T_SENS			0x02	// тип датчика
#define 	EC_S_INOUT			0x03	// 0 - датчики, 1 - исполнительные устройства, default - не читать


#endif /* CAN_PAR_ID_H_ */
