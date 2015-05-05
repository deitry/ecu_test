/*
 * engine.cpp
 *
 *  Created on: 15.09.2014
 *      Author: Дима
 */

#include "engine.h"
#include "math.h"

using namespace EG;

int fdbkCnt;
PAR_ID_BYTES pid = canTransmitId[cntCanTransmit = 0];

/**
 * Базовый конструктор.
 */
//#pragma CODE_SECTION("ramfuncs")
EC_Engine::EC_Engine()
{
	// константы и около того
	TenvMin = 300;
	TcoolMin = 300;

	this->tCool = 0;

	this->mode = EC_Start;
	// инициализация вложенных объектов
	//list = new EC_DeviceList();
	hard = new EC_Hardware();
	hard->Initialise();
}

/**
 * Проверяет состояние двигателя, системы управления, осуществляет индикацию...
 * Возможно, здесь же, параллельно, должно проверяться состояние органов управления...
 *
 * Сейчас : Отправляет на CAN сообщения со значениями параметров состояния системы
 *
 * В зависимости от имеющегося времени на выполнение и потребности в проверке,
 * можно выделить несколько функций мониторинга, каждая из которых "расширяет полномочия"
 * предыдущей. То есть первая проверяет только самое главное, вторая самое и не самое
 * главное, третья проверяет всё, четвёртая - всё и даже больше.
 *
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Monitoring(void)
{
	// Общий режим оцифровки датчиков
	if (!manSens)
	{
		getSensor(sensP, chanP, valP);
		Pk = Pk_koeff*valP;

		getSensor(sensT, chanT, valT);
		Tv = Tv_koeff*valT;
	}

	if (canSend)
	{
		if (!sendCanMsg(pid)) // (unsigned char*) &this->mode
		{
			// 0 - сan свободен, сообщение отправлено
			if (manFdbk && (canTransmitId[cntCanTransmit].P == EC_P_M_FDBK) && (pid.S < FDBK_BUF))
			{
				fdbkLock = 1;
				pid.S++;
			} else {
				cntCanTransmit++;
				// если дошли до конца списка - сбрасываем
				if (cntCanTransmit == PARIDMAX) // PARIDMAX
				{
					cntCanTransmit = 0;
				}
				pid = canTransmitId[cntCanTransmit];
				fdbkLock = 0;
			}
		}
	}

	switch (inOut)
	{
	case 0:
		for (int i = 0; i < SENSCNT; i++)
		{
			getSensor(sensX[i], chanX[i], valX[i]);
		}
		break;
	case 1:
		for (int i = 0; i < SENSCNT; i++)
		{
			setSensor(sensX[i], chanX[i], valX[i]);
		}
		break;
	default:
		break;
	}

	return 0;
}

#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setQCrelay()
{
	if (err < -errRelayMax)
	{
		QC = QCmin;
	} else {
		setQCTrans();
	}
}

/**
 * Перерасчёт режима работы.
 * На данный момент под этим подразумевается опрос всех датчиков и изменение
 * регулирующих воздействий согласно потребностям.
 *
 * ================================================================================================
 *
 * На вход в setInjector() будет поступать определённое значение g_step2Us,
 * которое уже прошло прошло всевозможные проверки и ограничения.
 * Поэтому пересчёт QC в мкс должен осуществляться в самом конце ModeCalc(),
 * общие ограничения д.б. вынесены непосредственно перед пересчётом, а в процедурах setQCxxx()
 * должен осуществляться расчёт подачи согласно установленному расчёту БЕЗ ограничений
 *
 * ================================================================================================
 *
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::ModeCalc()
{
	if (!manQC)
	{
		// корректируем количество топлива в зависимости от режима.
		switch (this->mode)
		{
		// стартовое количество топлива
		case EC_Start:
			QC = EGD::QCStart->get(Tcool());
			break;

		// холостой ход - поддержание - ПИД
		case EC_Steady:
			// уставка в зависимости от температуры охлаждающей жидкости
			nU = EGD::NDSteady->get(Tcool()) / HMLTP;
			setQCTrans();
			break;

		// рабочий режим - ПИД поддерживает частоту вращения
		case EC_Transient:
			// - считывание уставки - положение педали
			if (!manPed)
				pedal = Pedal();

			// ограничение темпа набора
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			// Релейный закон, если пожелаем. Он сбрасывает резко подачу при положительном изменении
			// выше некоторого значения (errRelayMax)
			if (manQCRelay)
			{
				setQCrelay();
			} else {
				setQCTrans();
			}
			break;

		// рабочий режим - ПИД поддерживает частоту вращения
		case EC_Failure:
			// - считывание уставки - положение педали
			if (!manPed)
				pedal = Pedal();

			// ограничение темпа набора
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			setQCTrans();
			break;

		// переходный режим - по педали
		case EC_Automotive:
			QC = Pedal()/100 * QCmax; // в предположении, что педаль от 0 до 100 ...
			// QCmax
			break;

		case EC_Finish:
			pedal = 400/HMLTP;

			// ограничение темпа набора
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			setQCTrans();
			break;
		}

		// ОГРАНИЧЕНИЯ ПОДАЧИ
		// 1. Скоростная характеристика
		QCmax = EGD::SpChar->get(nR*HMLTP);
		// 2. Пневмокоррекция
		QCadop = Pk*18.29/(alphaDop*287.*Tv*14.3);

		// ПРИМЕНЕНИЕ ОГРАНИЧЕНИЙ
		// - для QCmax выбираем минимальное
		QCmax = ((QCmax < QCadop) ? QCmax : QCadop);
		if (QC > QCmax) QC = QCmax;
		if (QC < QCmin) QC = QCmin;

		// установка значений для впрыска
		g_step2Us = QCtoUS(QC);
	}
	else
	{
		// ручной режим
		if (manAngle)
		{
			g_step2Us = angleToTime(injAngle);
		}
		else
		{
			if (!manQCt)
			{
				g_step2Us = QCtoUS(QC);
			}
		}
	}


	setInjector(g_step1Us, g_step2Us, g_duty1, g_duty2);
	for (int i = 0; i < DIESEL_T_SETUP; i++)
		asm(" NOP ");

	n1 = (g_step1Us - g_step3Us/2)/TIM2_DIV;
	n2 = g_step3Us/TIM2_DIV;
	n3 = g_step2Us/TIM2_DIV;
	d1 = n1/FDBK_C1;
	d2 = n2/FDBK_C2;
	d3 = n3/FDBK_C3;

	return 0;
}

/**
 * Посылка абстрактного кода, драйвер блока сам выбирает что кому отправлять
 */
/*int EC_Engine::Indicate(int code)
{
	return 0;
}*/

/**
 * Опрос датчика температуры окружающей среды
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tenv()
{
	float32 temp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, temp); // NI: Channel 1
	return 0.9*temp/2;
}

/**
 * Опрос датчика температуры охлаждающей жидкости
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Tcool()
{
	return tCool;
}

/**
 * Опрос положения пускового ключа
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Key()
{
	float32 tmp;
	DIESEL_STATUS getTempStatus1 = getSensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_1, tmp); // NI: Channel 1
	return (floor(tmp * 2.76 / 1 + 0.5) * 1 - 100)/10;
}

/**
 * Опрос положения педали
 */
//#pragma CODE_SECTION("ramfuncs")
float EC_Engine::Pedal()
{
	float32 pedValue1;
	DIESEL_STATUS getTempStatus1 = getSensor(sens, chan, pedValue1); // NI: Channel 1  // TEMP_SENS_CHANNEL_2
	return floor(pedValue1 * 2.77 * 5 / HMLTP / pedStep + 0.5) * pedStep; //
}

/**
 * Перевод значения цикловой подачи в продолжительность удерживающего импульса в мкс
 */
//#pragma CODE_SECTION("ramfuncs")
Uint16 EC_Engine::QCtoUS(float qc)
{
	int tmp = qc/nR*1e9*kQc/HMLTP;
	//int tmp = qc*1e3*1e6/370; // * kmagic;
	if (tmp > 0)
	{
		Uint16 step2 = tmp;
		return step2;
	}
	else return 0;
}

/**
 * Коррекция углов поворота. Пересчитываем, учитываем поправки и угол опережения впрыска
 */
//#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setInjPhi(void)
{
	for (int ii = 0; ii < DIESEL_N_CYL; ii++)
	{
		if (!EG::manPhi)
		{
			// угол впрыска с учётом угла опережения и поправкой на форсирующий импульс
			injPhi[ii] = ii * DIESEL_PHI_MAX/DIESEL_N_CYL
					- EG::g_step1Us*6*EG::nR/S2US;	// /HMLTP ? какая у нас частота вращения?

			// угол опережения
			if (!EG::manOUVT)
			{
				if (this->mode == EC_Start)
				{
					// на старте фиксированный угол опережения 5 градусов
					injOuvt = -5 / HMLTP;
				}
				else
				{
					injOuvt = EGD::OUVT->get(EG::QC, EG::nR*HMLTP) / HMLTP;
				}
			}
			injPhi[ii] += injOuvt + anVMT;
		}


		// Приводим значение угла впрыска к разумному диапазону
		while (injPhi[ii] < 1)
		{
			injPhi[ii] += DIESEL_PHI_MAX;
		}
		while (injPhi[ii] >= DIESEL_PHI_MAX+1)
		{
			injPhi[ii] -= DIESEL_PHI_MAX;
		}
		// находим номер зуба
		int zii = injPhi[ii] / DIESEL_Z_PHI;

		// Делаем так, чтобы время от фронта зуба (где мы решаем впрыскивать)
		// до момента впрыска не было равно нулю и около того.
		if (!((int)(injPhi[ii]) % DIESEL_Z_PHI))
		{
			zii--;
		}

		// стараемся не попасть в вырезанные зубья
		if (zii >= DIESEL_Z_ALL*2-DIESEL_Z_CUT-1)
		{
			zii = DIESEL_Z_ALL*2-DIESEL_Z_CUT-1;
		}
		else if ((zii >= DIESEL_Z_MAX) && (zii < DIESEL_Z_ALL))
		{
			zii = DIESEL_Z_MAX-1;
		}

		// записываем окончательно вычисленное значение
		injZ[ii] = zii;
		// находим разницу
		dTime[ii] = (injPhi[ii] - injZ[ii]*DIESEL_Z_PHI)*3.14/180/omegaR*S2US;	// мкc
	}
}

/**
 * Проверка системы в целом
 */
/*int EC_Engine::SysCheck()
{
	return this->list->Check();
}*/

/**
 * Проверка системы управления - всё ли работает, все ли данные загружены и т.д.
 * Заодно - переключение режимов
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::ControlCheck()
{
	//int pedal;
	DIESEL_STATUS stat;

	if (!manMode)
	{
		// значение режима определяем по резистивному датчику
		switch (Key())

		{
		case 1:
			mode = EC_Start; break;
		case 2:
			mode = EC_Steady; break;
		case 3:
			mode = EC_Transient; break;
		case 4:
			mode = EC_Automotive; break;
		case 5:
			mode = EC_Econom; break;
		case 6:
			mode = EC_MaxDyn; break;
		case 7:
			mode = EC_Finish; break;
		case 8:
			mode = EC_Failure; break;
		case 9:
			mode = EC_Crash; break;
		default:
			mode = EC_Failure;
		}
	}

	return 0;
}

/**
 * Отправка сообщения на CAN
 */
#pragma CODE_SECTION("ramfuncs")
void sendCanMsgX(CAN_DATA* data)
{
	tCANMsgObject canSendMessage;

	unsigned char dataQ[8];
	for (int i = 0; i < 8; i++)
	{
		dataQ[i] = 0;
	}

	dataQ[0] = data->f.PARID;
	dataQ[1] = data->f.R1;
	dataQ[2] = data->f.R2;
	dataQ[3] = data->f.R3;

	for (int j = 0; j < 2; j++)
	{
		dataQ[4+j*2] = (data->all[4+j] << 8) >> 8;
		dataQ[4+j*2+1] = data->all[4+j]>>8;
	}

	canSendMessage.ui32MsgID = 1;                   // CAN message ID - use 1
	canSendMessage.ui32MsgIDMask = 0;               // no mask needed for TX
	canSendMessage.ui32Flags = MSG_OBJ_NO_FLAGS;    // no flags
	canSendMessage.ui32MsgLen = 8;     				// size of message is 8
	canSendMessage.pucMsgData = dataQ;     			// ptr to message content

	// ждём, пока не освободиться место
	if (!canLock)
	{
		canWrite(&canSendMessage);
		canLock = canLockM;
	}
}

/**
 * Отправка сообщения на CAN
 */
#pragma CODE_SECTION("ramfuncs")
void sendCanMsg(int id, float* val)
{
	CAN_DATA data;

	for (int i = 0; i < 6; i++)
	{
		data.all[i] = 0;
	}

	data.f.PARID = id;
	data.f.val.f = *val;
	sendCanMsgX(&data);
}
void sendCanMsg(int id, Uint32* val)
{
	CAN_DATA data;

	for (int i = 0; i < 6; i++)
	{
		data.all[i] = 0;
	}

	data.f.PARID = id;
	data.f.val.i = *val;
	sendCanMsgX(&data);
}

#pragma CODE_SECTION("ramfuncs")
int EC_Engine::sendCanMsg(PAR_ID_BYTES id)
{
	if (canLock) return 1;

	CAN_DATA data;

	for (int i = 0; i < 6; i++)
	{
		data.all[i] = 0;
	}

	data.f.PARID = id.P;
	data.f.R1 = id.S;

	switch (id.P)
	{
	case EC_TPROG:
	case EC_TINJ:
	case EC_TIME:
		// Передаём временную метку
		// Она складывается из:
		// - номер оборота Uint16
		// - номер зуба Uint8
		// - счётчик времени Uint32
		data.f.R1 = zCnt;
		data.f.R2 = (nCnt << 8) >> 8;
		data.f.R3 = nCnt>>8;;
		data.f.val.i = timeCnt;
		break;
	case EC_P_MODE: data.f.val.i = this->mode; break;
	case EC_G_N:
		switch (id.S)
		{
		case EC_S_NR: data.f.val.f = EG::nR; break;
		case EC_S_NU: data.f.val.f = EG::nU; break;
		case EC_S_OMEGA: data.f.val.f = EG::omegaR; break;
		case EC_S_DTIME: data.f.val.f = delta_time; break;
		}
		break;
	case EC_G_QC:
		switch (id.S)
		{
		case EC_S_QC_T: data.f.val.i = EG::g_step2Us; break;
		case EC_S_QC_AN: data.f.val.f = EG::injAngle; break;
		case EC_S_QC: data.f.val.f = EG::QC; break;
		case EC_S_ADOP: data.f.val.f = EG::alphaDop; break;
		case EC_S_QC_ADOP: data.f.val.f = EG::QCmax; break;
		}
		break;
	case EC_P_NCYL: data.f.val.i = DIESEL_N_CYL; break;
	case EC_P_PED:
		switch (id.S)
		{
		case EC_P0: data.f.val.f = EG::pedal; break;
		case EC_S_PEDST: data.f.val.i = EG::pedStep; break;
		}
		break;
	case EC_P_VMT: data.f.val.f = EG::anVMT; break;
	case EC_G_INJ:
		switch (id.S)
		{
		case EC_S_INJT1: data.f.val.i = EG::g_step1Us; break;
		case EC_S_INJT2: data.f.val.i = EG::g_step2Us; break;
		case EC_S_INJD1: data.f.val.i = EG::g_duty1; break;
		case EC_S_INJD2: data.f.val.i = EG::g_duty2; break;
		}
		break;
	case EC_T_INJPHI: data.f.val.f = EG::injPhi[id.S]; break;
	case EC_T_INJZ:	data.f.val.f = EG::injZ[id.S]; break;
	case EC_T_INJN:	data.f.val.f = EG::injN[id.S]; break;
	case EC_T_INJT:	data.f.val.f = EG::dTime[id.S]; break;
	case EC_P_M_MODE: data.f.val.i = EG::manMode; break;
	case EC_P_M_QC:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manQC; break;
		case EC_S_M_AN: data.f.val.i = EG::manAngle; break;
		case EC_S_M_IAN: data.f.val.f = EG::injAngle; break;
		case EC_S_M_QCT: data.f.val.i = EG::manQCt; break;
		}
		break;
	case EC_P_M_INJ:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manInj; break;
		case EC_S_M_IONCE: data.f.val.i = EG::injOnce; break;
		case EC_S_M_IN: data.f.val.i = EG::manN; break;
		}
		break;
	case EC_P_M_PED: data.f.val.i = EG::manPed; break;
	case EC_P_M_CANS:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::canSend; break;
		case EC_S_M_CANST: data.f.val.i = EG::canTime; break;
		}
		break;
	case EC_P_M_UOVT:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manOUVT; break;
		case EC_S_M_UOVT: data.f.val.f = EG::injOuvt; break;
		}
		break;
	case EC_P_M_FDBK:
		if (id.S == EC_P0)
		{
			data.f.val.i = EG::manFdbk;
		} else {
			data.f.val.f = EG::fdbkBuf[id.S - 1];
		}
		break;
	case EC_P_M_LED:
		data.f.val.i = EG::manLed;
		break;
	case EC_P_M_SENS:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manSens; break;
		case EC_S_PK: data.f.val.f = EG::Pk; break;
		case EC_S_TV: data.f.val.f = EG::Tv; break;
		}
		break;
	case EC_P_KP:
		data.f.val.f = EG::kP;
		break;
	case EC_P_KI:
		data.f.val.f = EG::kI;
		break;
	case EC_P_KD:
		data.f.val.f = EG::kD;
		break;
	case EC_P_ERR:
		data.f.val.f = EG::err*EG::kP;
		break;
	case EC_P_ERRI:
		data.f.val.f = EG::errI*EG::kI;
		break;
	case EC_P_ERRD:
		data.f.val.f = EG::errD*EG::kD;
		break;
	case EC_P_PTIME:
		data.f.val.f = EG::prog_time;
		break;
	case EC_P_ITIME:
		data.f.val.f = EG::int_time;
		break;
	case EC_P_ITIME1:
		data.f.val.f = EG::int_time1;
		break;
	case EC_P_MUN:
		data.f.val.f = EG::muN;
		break;
	case EC_P_DZONE:
		data.f.val.f = EG::dZone;
		break;
	case EC_G_CHAN:
		// чтение произвольных каналов
		switch (id.S)
		{
		// TODO : поддержка третьего индекса
		case EC_T_VAL:
			break;
		case EC_T_CHAN:
			break;
		case EC_T_SENS:
			break;
		case EC_S_INOUT:
			data.f.val.i = EG::inOut;
			break;
		}
		break; 
	}

	sendCanMsgX(&data);
	return 0;
}


#pragma CODE_SECTION("ramfuncs")
void EC_Engine::recieveCanMsg(tCANMsgObject* msg)
{
	for (tmpi = 0; tmpi < 2; tmpi++)
	{
		can_data.all[4+tmpi] = msg->pucMsgData[4+tmpi*2]
							+ (msg->pucMsgData[4+tmpi*2+1]<<8);
	}

	switch (msg->pucMsgData[0])
	{
	case EC_PREQ: // запрос параметра
		tparid.P = msg->pucMsgData[1];
		tparid.S = msg->pucMsgData[2];
		if (sendCanMsg(tparid))
		{
			canLock *= 500;
			while (--canLock);
			sendCanMsg(tparid);
		}
		break;
	case EC_P_MODE:
	{
		if (can_data.f.val.i <= EC_MAX_MODE)
		{
			this->mode = static_cast<eng_mod>(can_data.f.val.i);
		}
		break;
	}
	case EC_G_N:
		switch (msg->pucMsgData[1])
		{
		case EC_S_NR: EG::nR = can_data.f.val.f; break;
		case EC_S_NU: EG::nU = can_data.f.val.f; break;
		case EC_S_OMEGA: EG::omegaR = can_data.f.val.f; break;
		case EC_S_DTIME: delta_time = can_data.f.val.f; break;
		}
		break;
	case EC_G_QC:
		switch (msg->pucMsgData[1])
		{
		case EC_S_QC_T: EG::g_step2Us = can_data.f.val.i; break;
		case EC_S_QC_AN: EG::injAngle = can_data.f.val.f; break;
		case EC_S_QC: EG::QC = can_data.f.val.f; break;
		case EC_S_ADOP: EG::alphaDop = can_data.f.val.f; break;
		case EC_S_QC_ADOP: EG::QCmax = can_data.f.val.f; break;
		}
		break;
	case EC_P_PED:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::pedal = can_data.f.val.f; break;
		case EC_S_PEDST: EG::pedStep = can_data.f.val.i; break;
		}
		break;
	case EC_P_VMT: EG::anVMT = can_data.f.val.f; break;
	case EC_G_INJ:
		switch (msg->pucMsgData[1])
		{
		case EC_S_INJT1: EG::g_step1Us = can_data.f.val.i; break;
		case EC_S_INJT2: EG::g_step2Us = can_data.f.val.i; break;
		case EC_S_INJD1: EG::g_duty1 = can_data.f.val.i; break;
		case EC_S_INJD2: EG::g_duty2 = can_data.f.val.i; break;
		}
		break;
	case EC_T_INJPHI: EG::injPhi[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJZ:	EG::injZ[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJN:	EG::injN[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJT:	EG::dTime[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_P_M_MODE: EG::manMode = can_data.f.val.i; break;
	case EC_P_M_QC:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manQC = can_data.f.val.i; break;
		case EC_S_M_AN: EG::manAngle = can_data.f.val.i; break;
		case EC_S_M_IAN: EG::injAngle = can_data.f.val.f; break;
		case EC_S_M_QCT: EG::manQCt = can_data.f.val.i; break;
		}
		break;
	case EC_P_M_INJ:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manInj = can_data.f.val.i; break;
		case EC_S_M_IONCE: EG::injOnce = can_data.f.val.i; break;
		case EC_S_M_IN: EG::manN = can_data.f.val.i; break;
		}
		break;
	case EC_P_M_PED: EG::manPed = can_data.f.val.i; break;
	case EC_P_M_CANS:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::canSend = can_data.f.val.i; break;
		case EC_S_M_CANST: EG::canTime = can_data.f.val.i; break;
		}
		break;
	case EC_P_M_UOVT:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manOUVT = can_data.f.val.i; break;
		case EC_S_M_UOVT: EG::injOuvt = can_data.f.val.f; break;
		}
		break;
	case EC_P_M_FDBK:
		EG::manFdbk = can_data.f.val.i;
		break;
	case EC_P_M_LED:
		EG::manLed = can_data.f.val.i;
		break;
	case EC_P_M_SENS:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manSens = can_data.f.val.i; break;
		case EC_S_PK: EG::Pk = can_data.f.val.f; break;
		case EC_S_TV: EG::Tv = can_data.f.val.f; break;
		}
		break;
	case EC_P_KP:
		EG::kP = can_data.f.val.f;
		break;
	case EC_P_KI:
		if (EG::kI != 0)
		{
			EG::errI *= can_data.f.val.f/EG::kI;
		}
		EG::kI = can_data.f.val.f;
		break;
	case EC_P_KD:
		EG::kD = can_data.f.val.f;
		break;
	case EC_P_ERR:
		EG::err = can_data.f.val.f/(EG::kP == 0 ? 1 : EG::kP);
		break;
	case EC_P_ERRI:
		EG::errI = can_data.f.val.f/(EG::kI == 0 ? 1 : EG::kP);
		break;
	case EC_P_ERRD:
		EG::errD = can_data.f.val.f/(EG::kD == 0 ? 1 : EG::kP);
		break;
	case EC_P_MUN:
		EG::muN = can_data.f.val.f;
		break;
	case EC_P_DZONE:
		EG::dZone = can_data.f.val.f;
		break;
	case EC_G_CHAN:
		switch (msg->pucMsgData[1])
		{
		// TODO : поддержка таблиц
		case EC_T_VAL:
			//msg->pucMsgData[2]
			break;
		case EC_T_CHAN:
			break;
		case EC_T_SENS:
			break;
		case EC_S_INOUT:
			EG::inOut = can_data.f.val.i;
			break;
		}
		break; 
	}
}

/**
 * Перевод угла в продолжительность удерживающего импульса в мкс - необходимо для задания подачи в углах
 */
//#pragma CODE_SECTION("ramfuncs")
float angleToTime(float angle)
{
	return angle/omegaR* PI/180 *S2US;
}

/**
 * Перевод продолжительности в угол.
 * Сейчас не используется.
 */
//#pragma CODE_SECTION("ramfuncs")
float timeToAngle(float time)
{
	return omegaR*time;
}
