/*
 * run.cpp
 *
 *  Created on: 11 мая 2015 г.
 *      Author: Например Андрей
 */

#include "../main.h"

/**
 * Основной цикл программы - единственный публичный режим.
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Run()
{
	// ОСНОВНОЙ цикл
	for (;;)
	{
		this->ControlCheck();
		this->Monitoring();

		switch (this->mode)
		{
		case EC_Start:	// ПУСК
			{
				this->Start();
				break;
			}
		case EC_Steady:	// СТАЦИОНАРНЫЙ режим (холостой ход)
			{
				this->Steady();
				break;
			}
		case EC_MaxDyn:
		case EC_Automotive:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-подача
		case EC_Transient:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-уставка
			{
				this->Transient();
				break;
			}
		case EC_Failure:	// АВАРИЙНЫЙ режим (аварийное завершение работы?)
			{
				this->Failure();
				break;
			}
		case EC_Finish:	// ЗАВЕРШЕНИЕ работы (нормальное)
			{
				this->Finish();
				break;
			}
		case EC_End:
			{
				// подвисаем
				for (;;)
				{
					if (this->mode != EC_End)
						break;
				}
			}
		}
	}
}


