#include "../ecu_global_defines.h"
#include "interrupts.h"
#include "../main.h"
#include "math.h"
#include "F28x_Project.h"	// Device Headerfile and Examples Include File

Uint16 resultsDac = 0;
Uint16 resultsIndexA = 0;
Uint16 resultsIndexD = 0;
Uint16 ToggleCount = 0;
Uint16 dacOffset = 0;
Uint16 dacOutputB = 0;
Uint16 dacOutputA = 0;
Uint16 modeDAC = 0;

float delta_prev = 0.0;
float omegaN = 0.0;	// new
float omegaP = 0.0;	// previous
float delta_phi = 6*3.14/180;	// расстояние между фронтами зубьев
float mu = 0.95;
Uint16 sens = 0;
Uint16 prev = 0;
Uint16 tmp11 = 0;
Uint32 phit_offset = 0;
Uint32 phi_offset = 30;
float kOmega = 1.0;
Uint32 tCnt = 0;
int noCyc = 0;
int sw = 0;
float errN;
float omegaN1;

int i1, ii1, i2, i_fdbk;


using namespace EG;


/**
 * Прерывание adc-a 1
 * Отлов показаний датчиков AO0
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void adca1_isr(void)
{
	// Return from interrupt
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; 		// Clear INT1 flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;		// acknowledge PIE group 1 to enable further interrupts
}

/**
 * Прерывание adc-d 1
 * Отлов показаний датчиков - AO1
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void adcd1_isr(void)
{
	// Return from interrupt
	AdcdRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; 		// Clear INT1 flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;		// acknowledge PIE group 1 to enable further interrupts
}

/*if (manDur)
	{
		GpioDataRegs.GPADAT.bit.GPIO2 = 1;
	}
	// 162, 01, 02
*/

/**
 * Датчик распредвала (редкий)
 * - синхронизация впрыска
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void xint1_isr(void)
{
	if (manLed == 1)
	{
		cpldLedSwitch(LED_YELLOW, 1);
		GpioDataRegs.GPFSET.bit.GPIO162 = 1;
	}
	// замер времени между текущим и последним фронтом
	delta_prev = delta_time;
	delta_time = ((float) (CpuTimer0Regs.PRD.all-CpuTimer0Regs.TIM.all))
					/ (S2US*TIMER_FREQ);
	CpuTimer0Regs.TCR.bit.TRB = 1;	// сбрасываем таймер

	if (delta_time == 0)
	{
		delta_time = delta_prev;
	}

	// вычисление текущей частоты вращения
	// - не считаем, когда пропущены зубья
	// - частота вращения вычисляется для того вала, на котором установлен датчик Холла
	if (((delta_time / 1.4) < delta_prev)		// пропущенные зубья
			&& (zCnt <= (DIESEL_Z_ALL - 1))) 	// если пропуска зуба не обнаружено
	{
		zCnt++;
		omegaN = (float) (DIESEL_Z_PHI*PI/180. / delta_time);

		// "сглаживание" частоты вращения
		omegaN1 = omegaP*mu + omegaN*(1-mu);
		omegaR = (omegaN1 < 10000) ? omegaN1 : 10000;
		dOmega = (omegaN - omegaP)/delta_time;	// производная
		omegaP = omegaR;

		nR = omegaR*30/PI;	// пересчёт в об/мин

		errN = nU - nR;		// текущая ошибка
		errD = (errN - err) / delta_time;	// производная от ошибки

		// считаем интегральную составляющую с учётом ограничения
		// В качестве ограничения выбрано минимальное из QCmax (константа) и QCadop (по альфе) [engine.cpp - ModeCalc()]
		if ((fabs((errI + errN*delta_time)*kI) <= QCmax))	// && (fabs(dOmega) < errDMax)
		{
			errI = errI + errN*delta_time;
		} else if (fabs(errI*kI) > QCmax) {
			// чтобы предотвратить выбег интегральной составляющей при резком изменении QCmax,
			// приравниваем интегральную ошибку ограничению с учётом текущего знака
			errI *= QCmax / kI / fabs(errI);
		}

		err = errN;
	}
	else
	{
		if (manLed > 1)
		{
			cpldLedSwitch(LED_YELLOW, 1);
			GpioDataRegs.GPFSET.bit.GPIO162 = 1;
		}
		zCnt = 0;	// обнуляем счётчик зубов

		// чередование циклов коленчатого вала
		if (HALL_CRANKSHAFT)
		{
			sw = !sw;
		} else {
			sw = 0;
		}

		nCnt++;
	}

	// если не переключились в режим ручного впрыска
	if (!manInj)
	{
		for (ii1 = 0; ii1 < DIESEL_N_CYL; ii1++)
		{
			if (EG::injZ[ii1] == (zCnt + sw*DIESEL_Z_ALL))
			{
				injCnt[ii1] = 0;
				injSw[ii1] = 1;
			}
		}
	}

	// Отправлять по кану временную метку + номер оборота + номер зуба
	// или формировать пакет, а отправлять уже из основной программы
	if (canTime)
	{
		tPid.P = EC_TIME;
		engine->sendCanMsg(tPid);
	}

	if (manLed)
	{
		cpldLedSwitch(LED_YELLOW, 0);
		GpioDataRegs.GPFCLEAR.bit.GPIO162 = 1;
	}

	if (manDur)
	{
		int_time = ((float) (CpuTimer0Regs.PRD.all-CpuTimer0Regs.TIM.all))
									/ (S2US*TIMER_FREQ);
	}

	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/**
 * Датчик частоты коленвала (частый)
 * - измерение частоты вращения
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void xint2_isr(void)
{
	if (!sw) sw = 1;
	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION("ramfuncs")
__interrupt void cpu_timer1_isr(void)
{
	if (manLed)
	{
		cpldLedSwitch(LED_RED_2, 1);
	}


	if (manInj)
	{
		manCnt++;
		if (injOnce && (manN? (manCnt == manN) : 1))
		{
			startInjector(cylToCode(injCyl));
			if (manN)
			{
				manCnt = 0;
			} else {
				injOnce = 0;
			}
		}
	} else {
		for (i1 = 0; i1 < DIESEL_N_CYL; i1++)
		{
			if (injSw[i1])
			{
				injCnt[i1]++;
				if (injCnt[i1] >= dTime[i1]/TIM1_DIV) // && (QC > QCmin)
				// для реального двигателя надо будет не_впрыскивать, если мы сидим на минимуме.
				// но для работы имитатора необходимо впрыскивать всегда
				{
					startInjector(cylToCode(i1));

					// отправка временной метки о впрыске
					if (canTime)
					{
						tPid.P = EC_TINJ;
						engine->sendCanMsg(tPid);
					}
				}
			}

		}
	}

	if (manLed)
	{
		cpldLedSwitch(LED_RED_2, 0);
	}

	if (manDur)
	{
		int_time1 = ((float) (CpuTimer1Regs.PRD.all - CpuTimer1Regs.TIM.all))
							/ (S2US*TIMER_FREQ);
	}

	// Acknowledge this __interrupt to receive more __interrupts from group 1
	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/**
 * Прерывание по CAN.
 * Вписать сюда обработку принимаемых сообщений - изменение режимов и т.д.
 *
 * 8 байт - 4 байта идентификатор, 4 байта переменная
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void canHwIsr()
{
	unsigned long ulStatus;

	//if (manLed)
	//{
	//	setSensor(ANALOG_OUTPUT, ANALOG_OUT_CHANNEL_4, 4);
	//}

    // Read the CAN interrupt status to find the cause of the interrupt
    ulStatus = canGetIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    // If the cause is a controller status interrupt, then get the status
    if(ulStatus == CAN_INT_INT0ID_STATUS)
    {
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.  If the
        // CAN peripheral is not connected to a CAN bus with other CAN devices
        // present, then errors will occur and will be indicated in the
        // controller status.
        ulStatus = canGetStatus(CANA_BASE, CAN_STS_CONTROL);

        //Check to see if an error occured.
        if(((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 7) &&
        		((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 0)){
        	// Set a flag to indicate some errors may have occurred.
        	//printf("Error occured");
        }
    }

    // Check if the cause is message object 1, which what we are using for
    // sending messages.
    /*else if(ulStatus == 1)
    {
        // Getting to this point means that the TX interrupt occurred on
        // message object 1, and the message TX is complete.  Clear the
        // message object interrupt.
        CANIntClear(CANA_BASE, 1);

        // Increment a counter to keep track of how many messages have been
        // sent.  In a real application this could be used to set flags to
        // indicate when a message is sent.
        g_ulTxMsgCount++;

        // Since the message was sent, clear any error flags.
        g_bErrFlag = 0;
    }*/

    // Check if the cause is message object 1, which what we are using for
    // recieving messages.
    else if(ulStatus == 2)
    {

    	// Get the received message
		canRead(canReceiveMessage);

        // Getting to this point means that the TX interrupt occurred on
        // message object 1, and the message TX is complete.  Clear the
        // message object interrupt.
        canIntClear(CANA_BASE, 2);

        engine->recieveCanMsg(&canReceiveMessage);
    }

    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.
    else
    {
        // Spurious interrupt handling can go here.
    }

    //DcanaRegs.CAN_GLB_INT_CLR.bit.INT0_FLG_CLR = 1;
    canGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);

    //if (manLed)
    //{
    //	setSensor(ANALOG_OUTPUT, ANALOG_OUT_CHANNEL_4, 0);
    //}

    PieCtrlRegs.PIEACK.all = 0x0100; //PIEACK_GROUP9;
}


#pragma CODE_SECTION("ramfuncs")
Uint16 cylToCode(int nCyl)
{
	switch (nCyl)
	{
	case 0: return 0x0001;
	case 1: return 0x0002;
	case 2: return 0x0004;
	case 3: return 0x0008;
	case 4: return 0x0010;
	case 5: return 0x0020;
	case 6: return 0x0040;
	case 7: return 0x0080;
	case 8: return 0x0100;
	case 9: return 0x0200;
	case 10: return 0x0400;
	case 11: return 0x0800;
	}
	return 0x0000;
}

Uint32 fdbk_time = 0;
int fdbk_flag = 0;

/**
 * Вспомогательный таймер для расчёта
 */
#pragma CODE_SECTION("ramfuncs")
__interrupt void cpu_timer2_isr(void)
{
	progCnt++;
	timeCnt++;
	if (canLock > 0)
		canLock--;

	fdbk_time = fdbkTCnt;

	if (getFdbk && (!fdbkLock))
	{
		if (fdbkTCnt < n1)
		{
			if (fdbkTCnt >= i_fdbk*n1/FDBK_C1)
				fdbk_flag = 1;
		}
		else if ((fdbkTCnt > n1) && (fdbkTCnt < (n1 + n2)))
		{
			if ((fdbkTCnt-n1) >= (i_fdbk-FDBK_C1)*n2/FDBK_C2)
				fdbk_flag = 1;
		}
		else if ((fdbkTCnt > (n1+n2)) && (fdbkTCnt < (n1+n2+n3)))
		{
			if ((fdbkTCnt-n1-n2) >= (i_fdbk-FDBK_C1-FDBK_C2)*n3/FDBK_C3)
				fdbk_flag = 1;
		}

		if (fdbk_flag)
		{
			getSensor(INJECTOR_SENSOR, cylToCode(fdbkChan), fdbkBuf[i_fdbk]);
			fdbkTBuf[i_fdbk] = fdbk_time;
			i_fdbk++;
			fdbk_flag = 0;

			if (i_fdbk >= FDBK_BUF)
			{
				getFdbk = 0;
				i_fdbk = 0;
				fdbkAll = 0;
			}
		}
	}

	fdbkTCnt++;

	// Acknowledge this __interrupt to receive more __interrupts from group 1
	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
