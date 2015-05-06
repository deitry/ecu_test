/*
 * transient.cpp
 *
 *  Created on: 12.09.2014
 *      Author: Дима
 */

#include "../main.h"
#include "math.h"


using namespace EG;

int ledCnt = 0;
int canStampCnt = 0;

/**
 * Переходный режим. Во время движения режим всегда переходный, т.к. постоянно изменяется
 * состояние органов управления...
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Transient()
{
	float32 tempValue1;
	for(;;)
	{
		if (manLed)
		{
			cpldLedToggle(LED_GREEN);

#ifdef IS_DOCKED
			ledCnt++;
			if (ledCnt == 200)
			{
				GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
				ledCnt = 0;
			}
#endif
		}

		if (canTime)
		{
			canStampCnt++;
			if (canStampCnt == 1000)
			{
				canStampCnt = 0;
				tPid.P = EC_TPROG;
				engine->sendCanMsg(tPid);
			}
		}
		// проверка состояния системы
		/*if (this->SysCheck())
		{
			// если что-то не так
			// переходим в режим Failure
			return 1;
		}*/

		// проверка состояния органов управления
		if (!this->ControlCheck())
		{
			// выработка нового режима - перерасчёт топлива и т.д.
			if (	(mode != EC_Transient)
				&& 	(mode != EC_Automotive)
					) return 0;
		}


		// ПОДДЕРЖАНИЕ НОВОГО РЕЖИМА
		// - коррекция углов впрыска
		setInjPhi();
		// - коррекция величины подачи
		this->ModeCalc();
		// мониторинг - отправка сообщений по CAN - сделать "параллельным процессом"?
		this->Monitoring();

		if (manDur)
		{
			prog_time = static_cast<float>(progCnt)	/ S2US * TIM2_DIV;
			progCnt = 0;	// сбрасываем таймер
		}
	}

	return 0;
}

#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setQCTrans(void)
{
	if (fabs(err) > EG::dZone)
	{
		// пид-закон
		QCprev = QC;
		QC = kP*err + kI*errI + kD*errD;
	}
}
