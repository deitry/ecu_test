/*
 * start.cpp
 *
 *  Created on: 12.09.2014
 *      Author: Дима
 */

#include "../main.h"
#include "math.h"


using namespace EG;

/**
 * Пусковые мероприятия.
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Start()
{
	// - до 30 об/мин ничего не происходит - режим ожидания
	if (nR._val < 30)
	{
		QC = 0;
		g_step2Us = 0;
		return 0;
	}

	// - до 150 об/мин - подача 500 мм3/цикл - не принимаем уставку снаружи
	if (nR._val < 150)
	{
		// - коррекция углов впрыска
		this->setInjPhi();
		// - коррекция величины подачи
		this->ModeCalc();
	}
	else
	{
		// - раскручиваем до 300 об/мин - можем начать принимать уставку снаружи
		pedal = 300;
		// - переходим на нормальный режим
		clearPID();
		mode = EC_Transient;
	}

	return 0;
}




void EC_Engine::setQCSteady(void)
{
	float QC1;

	if (fabs(err) > DIESEL_D_ZONE)
	{
		// пид-закон
		QC1 = kP*err + kI*errI + kD*errD;
		if (QC1 > QCmax) QC1 = QCmax;
		if (QC1 < QCmin) QC1 = QCmin;
		QC = QC1;

		if (!manQC)
			g_step2Us = QCtoUS(QC);

		// TODO : ограничение по Tcool - завести небольшую карту

		//setInjector(g_step1Us, g_step2Us, g_duty1, g_duty2);
		// ждём, пока установятся значения

	}
}
