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
		engine->ControlCheck();
		engine->Monitoring();

		switch (engine->mode)
		{
		case EC_Start:	// ПУСК
			{
				engine->Start();
				break;
			}
		case EC_Steady:	// СТАЦИОНАРНЫЙ режим (холостой ход)
			{
				engine->Steady();
				break;
			}
		case EC_Automotive:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-подача
		case EC_Transient:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-уставка
			{
				engine->Transient();
				break;
			}
		case EC_Failure:	// АВАРИЙНЫЙ режим (аварийное завершение работы?)
			{
				engine->Failure();
				break;
			}
		case EC_Finish:	// ЗАВЕРШЕНИЕ работы (нормальное)
			{
				engine->Finish();
				break;
			}
		case EC_End:
			{
				// подвисаем
				for (;;)
				{
					if (engine->mode != EC_End)
						break;
				}
			}
		}
	}
}


