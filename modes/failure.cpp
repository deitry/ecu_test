/*
 * failure.cpp
 *
 *  Created on: 12.09.2014
 *      Author: Дима
 */

#include "../main.h"
#include "math.h"

using namespace EG;

/**
 * Пока что копия трансиент, только ограничение по подаче в 2 раза меньше
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Failure()
{
	for(;;)
	{
		// проверка состояния органов управления
		if (!this->ControlCheck())
		{
			// выработка нового режима - перерасчёт топлива и т.д.
			//this->ModeCalc();
			if (mode != EC_Failure)
				return 0;
		}

		// - коррекция углов впрыска
		setInjPhi();
		// - коррекция величины подачи
		this->ModeCalc();
		this->Monitoring();
	}
}
