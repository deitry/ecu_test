/*
 * engine_can.cpp
 *
 *  Created on: 11 мая 2015 г.
 *      Author: Например Андрей
 */

#include "engine.h"


using namespace EG;

int fdbkCnt;
PAR_ID_BYTES pid = canTransmitId[cntCanTransmit = 0];

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
	// проверяем ключевые переменные на выход за пределы
	if (!nR.Check())
	{
		// останавливаем блок
		// пока - отключаем впрыски
		manInj = 1;
		injOnce = 0;

		// Предлагается сделать так:
		// при выходе за границы начинает работать "взвешенный" счётчик.
		// Системе даётся некоторое время (например, секунда, 1000 мс) и каждую мс
		// из общего времени вычитают по n мс, где n зависит от отклонения параметра.
		// Чем больше отклонение, тем быстрее вычитается время. Около границы по 1 мс,
		// дальше по 2, 5, 10, 100, 1000 (если введу вторую границу, то это будет даже
		// чётко обозначенная экспонента, иначе будем ориентироваться на %)
		// При возвращении в допустимый диапазон обнуление счётчика должно быть не мгновенным,
		// а постепенным - каждую мс нужно "возвращать" по мс.
	}

	// Общий режим оцифровки датчиков
	if (!manSens)
	{
		Pk = this->devices->getDevice(EC_S_D_PK)->getValue();
		Tv = this->devices->getDevice(EC_S_D_TV)->getValue();
	}

	if (canSend)
	{
		// НОВАЯ ВЕРСИЯ CAN
		if (elCanTransmit)
		{
			// - если есть параметр для "разовой отправки", отправляем его
			for (int i = 0; i < 2000; i++);

			if (singlePID.P != EC_BAD)
			{
				if (!sendCanMsg(singlePID))
				{
					singlePID.P = EC_BAD;
					singlePID.S = 0;
				}
			}
			// - отправляем сообщение с параметром, текущим в списке
			else if (!sendCanMsg(elCanTransmit->current))
			{
				// - переходим на следующий параметр
				// - если параметр "разовый", вычёркиваем его из списка
				//if (elCanTransmit->single)
				//{
				//	CANListElement* cur = elCanTransmit;
				//	elCanTransmit = elCanTransmit->next;
				//	delete cur;
				//}
				// Проблема перебора кучи чисел, соответствующих одному параметру, остаётся открытой.
				// Когда приспичит, можно будет сделать как раньше - изменяем индекс у current, относящемуся к данном PIB
				elCanTransmit = elCanTransmit->next;
				if (!elCanTransmit)
				{
					elCanTransmit = CANListElement::first;
				}

				// СТАРАЯ ВЕРСИЯ
				// 0 - сan свободен, сообщение отправлено
				/*if (manFdbk && (!fdbkAll) && (canTransmitId[cntCanTransmit].P == EC_P_M_FDBK) && (pid.S < FDBK_BUF))
				{
					fdbkLock = 1;
					pid.S++;
					if (pid.S == FDBK_BUF)
					{
						fdbkAll = 1;
					}
				} else {
					cntCanTransmit++;
					// если дошли до конца списка - сбрасываем
					if (cntCanTransmit == PARIDMAX)
					{
						cntCanTransmit = 0;
					}
					pid = canTransmitId[cntCanTransmit];
					fdbkLock = 0;
				}*/
			}
			for (int i = 0; i < 2000; i++);
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
	//if (!canLock)
	//{
		canWrite(&canSendMessage);
		canLock = canLockM;
	//}
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
		case EC_S_NR: data.f.val.f = EG::nR._val; break;
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
		case EC_S_QC_ADOP: data.f.val.f = EG::QCadop; break;
		case EC_S_QC_MAX: data.f.val.f = EG::QCmax; break;
		case EC_S_QC_MIN: data.f.val.f = EG::QCmin; break;
		case EC_S_KQC: data.f.val.f = EG::kQc; break;
		}
		break;
	case EC_P_NCYL:
		if (id.S == EC_P0)
		{
			data.f.val.i = DIESEL_N_CYL;
		}
		else
		{
			data.f.val.i = EG::cylChannel[id.S-1];
		}
		break;
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
	case EC_T_INJPHI: 	data.f.val.f = EG::injPhi[id.S]; break;
	case EC_T_INJZ:		data.f.val.f = EG::injZ[id.S]; break;
	case EC_T_INJN:		data.f.val.f = EG::injN[id.S]; break;
	case EC_T_INJT:		data.f.val.f = EG::dTime[id.S]; break;
	case EC_T_INJCNT:	data.f.val.i = EG::injCnt[id.S]; break;
	case EC_P_M_MODE: 	data.f.val.i = EG::manMode; break;
	case EC_P_M_QC:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manQC; break;
		//case EC_S_M_AN: data.f.val.i = EG::manAngle; break;
		case EC_S_M_IAN: data.f.val.f = EG::injAngle; break;
		//case EC_S_M_QCT: data.f.val.i = EG::manQCt; break;
		}
		break;
	case EC_P_M_INJ:
		switch (id.S)
		{
		case EC_P0: data.f.val.i = EG::manInj; break;
		case EC_S_M_IONCE: data.f.val.i = EG::injOnce; break;
		case EC_S_M_IN: data.f.val.i = EG::manN; break;
		case EC_S_M_INJCYL: data.f.val.i = EG::injCyl; break;
		case EC_S_FDBKCYL: data.f.val.i = EG::fdbkCyl; break;
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
		case EC_S_D_PK: data.f.val.f = EG::Pk; break;
		case EC_S_D_TV: data.f.val.f = EG::Tv; break;
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
		case EC_T_VAL:
			data.f.val.f = EG::valX[id.S2];
			break;
		case EC_T_CHAN:
			data.f.val.i = EG::chanX[id.S2];
			break;
		case EC_T_SENS:
			data.f.val.i = EG::sensX[id.S2];
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
	case EC_PQUE:	// добавление параметра в очередь
	{
		// - создаём новый объект
		PAR_ID_BYTES pid = {msg->pucMsgData[1],msg->pucMsgData[2]};
		CANListElement* newEl = new CANListElement(pid,	CANListElement::Last());
		if (!elCanTransmit)
		{
			elCanTransmit = CANListElement::first;
		}
		//
		break;
	}
	case EC_PCLR:
		CANListElement::Clear();
		break;
	case EC_PREQ: // запрос параметра
		//if (canSend)
		//{
		//	EG::canTransmitId[PARIDMAX-1].P = msg->pucMsgData[1];
		//	EG::canTransmitId[PARIDMAX-1].S = msg->pucMsgData[2];
		//}
		//else
	{
		//DELAY_US(1);
		singlePID.P = msg->pucMsgData[1];
		singlePID.S = msg->pucMsgData[2];
		//PAR_ID_BYTES pid = {,};
		//if (sendCanMsg(pid))
		//{
		//	if (canLock)
		//	{
		//		canLock *= 500;
		//		while (canLock--);
		//	}
		//	sendCanMsg(pid);
		//}
		//DELAY_US(1);
		break;
	}
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
		case EC_S_QC_ADOP: EG::QCadop = can_data.f.val.f; break;
		case EC_S_QC_MAX: EG::QCmax = can_data.f.val.f; break;
		case EC_S_QC_MIN: EG::QCmin = can_data.f.val.f; break;
		case EC_S_KQC: EG::kQc = can_data.f.val.f; break;
		}
		break;
	case EC_P_PED:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::pedal = can_data.f.val.f; break;
		case EC_S_PEDST: EG::pedStep = can_data.f.val.i; break;
		}
		break;
	case EC_P_NCYL:
		if (msg->pucMsgData[1] == EC_P0)
			break;
		EG::cylChannel[msg->pucMsgData[1]-1] = can_data.f.val.i;
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
	case EC_T_INJPHI: 	EG::injPhi[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJZ:		EG::injZ[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJN:		EG::injN[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJT:		EG::dTime[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_T_INJCNT:	EG::injCnt[msg->pucMsgData[1]] = can_data.f.val.f; break;
	case EC_P_M_MODE: 	EG::manMode = can_data.f.val.i; break;
	case EC_P_M_QC:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manQC = can_data.f.val.i; break;
		//case EC_S_M_AN: EG::manAngle = can_data.f.val.i; break;
		case EC_S_M_IAN: EG::injAngle = can_data.f.val.f; break;
		//case EC_S_M_QCT: EG::manQCt = can_data.f.val.i; break;
		}
		break;
	case EC_P_M_INJ:
		switch (msg->pucMsgData[1])
		{
		case EC_P0: EG::manInj = can_data.f.val.i; break;
		case EC_S_M_IONCE: EG::injOnce = can_data.f.val.i; break;
		case EC_S_M_IN: EG::manN = can_data.f.val.i; break;
		case EC_S_M_INJCYL: EG::injCyl = can_data.f.val.i; break;
		case EC_S_FDBKCYL: EG::fdbkCyl = can_data.f.val.i; break;
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
		case EC_S_D_PK: EG::Pk = can_data.f.val.f; break;
		case EC_S_D_TV: EG::Tv = can_data.f.val.f; break;
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
		EG::errI = can_data.f.val.f/(EG::kI == 0 ? 1 : EG::kI);
		break;
	case EC_P_ERRD:
		EG::errD = can_data.f.val.f/(EG::kD == 0 ? 1 : EG::kD);
		break;
	case EC_P_MUN:
		EG::muN = can_data.f.val.f;
		break;
	case EC_P_DZONE:
		EG::dZone = can_data.f.val.f;
		break;
	case EC_G_CHAN:
		// чтение произвольных каналов
		switch (msg->pucMsgData[1])
		{
		case EC_T_VAL:
			EG::valX[msg->pucMsgData[2]] = can_data.f.val.f;
			break;
		case EC_T_CHAN:
			EG::chanX[msg->pucMsgData[2]] = can_data.f.val.i;
			break;
		case EC_T_SENS:
			EG::sensX[msg->pucMsgData[2]] = can_data.f.val.i;
			break;
		case EC_S_INOUT:
			EG::inOut = can_data.f.val.i;
			break;
		}
		break;
	}
}


