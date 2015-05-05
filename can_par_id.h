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

/**
 * Запрос на получение параметра. Формат данных в таком случае:
 * (0xFF) - (P) - (S) - (SS) - (SSS)
 */
#define EC_PREQ		0xFF

#define EC_TIME		0xFE		// метка времени на момент прихода прерывания от зуба
#define EC_TINJ		0xFD		// метка времени на момент впрыска
#define EC_TPROG	0xFC		// метка времени на новом цикле программы

#define EC_P0		0x00

#define EC_BAD		0xFFFF
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

#define EC_P_MODE		0x01		// режим
#define EC_G_N			0x02
#define 	EC_S_NR				0x00
#define 	EC_S_NU				0x01
#define 	EC_S_OMEGA			0x02
#define 	EC_S_DTIME			0x03

#define EC_G_QC			0x03
#define 	EC_S_QC_T			0x00
#define 	EC_S_QC_AN			0x01
#define 	EC_S_QC				0x02
#define		EC_S_ADOP			0x03
#define		EC_S_QC_ADOP		0x04
#define EC_P_NCYL		0x04
#define EC_P_PED		0x05
#define 	EC_S_PEDST			0x01
#define EC_P_VMT		0x06		// угол смещения ВМТ относительно пропущенных зубьев

#define EC_G_INJ		0x10		// параметры впрыска
#define 	EC_S_INJT1			0x00
#define 	EC_S_INJT2			0x01
#define 	EC_S_INJD1			0x02
#define 	EC_S_INJD2			0x03

#define EC_T_INJPHI		0x11		// подиндекс - номер цилиндра 0..NCYL
#define EC_T_INJZ		0x12
#define EC_T_INJN		0x13
#define EC_T_INJT		0x14


#define EC_P_M_MODE		0x21
#define EC_P_M_QC		0x22
#define 	EC_S_M_AN			0x01
#define 	EC_S_M_IAN			0x02
#define		EC_S_M_QCT			0x03
#define EC_P_M_INJ		0x23
#define 	EC_S_M_IONCE		0x01
#define 	EC_S_M_IN			0x02
#define EC_P_M_PED		0x24
#define EC_P_M_CANS		0x25
#define EC_S_M_CANST			0x01		// передача меток времени
#define EC_P_M_UOVT		0x26
#define 	EC_S_M_UOVT			0x01
#define EC_P_M_FDBK		0x27
#define EC_P_M_LED		0x28
#define EC_P_M_SENS		0x29				// данные от оцифровки датчиков
#define 	EC_S_PK				0x01		// Pkompr
#define		EC_S_TV				0x02		// Tvozd

#define EC_T_UOVT		0x31	// таблица
#define EC_T_SPCHAR		0x32
#define EC_T_NSTEADY	0x33
#define EC_T_QSTART		0x34

#define EC_P_KP			0x41
#define EC_P_KI			0x42
#define EC_P_KD			0x43
#define EC_P_ERR		0x44
#define EC_P_ERRI		0x45
#define EC_P_ERRD		0x46

#define EC_P_MUN		0x47
#define EC_P_DZONE		0x48

#define EC_P_PTIME		0x51
#define EC_P_ITIME		0x52
#define EC_P_ITIME1		0x53

// Группа параметров, отвечающих за чтение произвольных каналов
#define EC_G_CHAN		0x61
#define 	EC_T_VAL		0x00
#define 	EC_T_CHAN		0x01
#define 	EC_T_SENS		0x02
#define 	EC_S_INOUT		0x03	// 0 - датчики, 1 - исполнительные устройства, default - не читать


#endif /* CAN_PAR_ID_H_ */
