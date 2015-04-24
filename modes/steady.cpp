/*
 * steady.cpp
 *
 *  Created on: 12.09.2014
 *      Author: Дима
 */

#include "../main.h"

using namespace EG;

/**
 * Стационарный режим. Предположительно будет использоваться только для поддержания
 * холостого хода, в иных случаях режим считается нестационарным...
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Steady()
{
	// TODO : pedal = 400, уставка пусть сама доползает
	nU = 400;	// TODO : частота холостого хода

	for(;;)
	{
		if (manLed)
		{
			cpldLedToggle(LED_GREEN);
		}

		// проверка состояния органов управления
		if (!this->ControlCheck())
		{
			// выработка нового режима - перерасчёт топлива и т.д.
			if (mode != EC_Steady) return 0;
		}

		// - коррекция углов впрыска
		setInjPhi();
		// - коррекция величины подачи
		this->ModeCalc();
		this->Monitoring();
	}
}


