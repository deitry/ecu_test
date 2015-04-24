/**
 * Пробный вариант реализации программы управления
 *
 * main.cpp
 */

#include "main.h"
//#include <memcopy.h>

//using namespace EG;
EC_Engine* engine;

//#pragma CODE_SECTION("ramfuncs")
int main(void) {
	
	// инициализация устройства
	//MemCopy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
	InitSysCtrl();

	// ИНИЦИАЛИЗАЦИЯ
	engine = new EC_Engine();
	engine->mode = EC_Transient;

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
				EG::errI = 0;	// сбрасываем интегральную составляющую
				engine->Steady();
				break;
			}
		case EC_Automotive:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-подача
		case EC_Transient:	// НЕСТАЦИОНАРНЫЙ (рабочий) режим - педаль-уставка
			{
				EG::errI = 0;	// сбрасываем интегральную составляющую
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
				// возвращаем коэффициенты на свои места
				//EG::kP = 1e-5;
				//EG::kI = 1e-6;
				//EG::kD = 0;
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
