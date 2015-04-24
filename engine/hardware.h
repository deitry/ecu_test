/**
 * hardware.h
 *
 * Содержит класс EC_Hardware, объединяющий функции работы с устройством: прерывания и проч.
 *
 * Возможно, не самый лучший подход, если понадобится, потом переделаю.
 *
 *  Created on: 29.10.2014
 *      Author: Дима
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

//#include "ecu_global_defines.h"
#include "interrupts.h"
#include "../drv/diesel_drv.h"
#include "F28x_Project.h"	// Device Headerfile and Examples Include File

//#define DELAY (CPU_RATE/1000*6*510)  //Qual period at 6 samples


/**
 * По сути, этот класс объединяет все возможные методы работы с оборудованием.
 * Лучше всего будет внести сюда же процедуры настройки. (?)
 */
class EC_Hardware {
public:
	EC_Hardware();			// базовый конструктор
	void Initialise(void);	// единая процедура настройки

protected:
	 // Процедуры для настройки устройства.
	void ConfigurePins(void);
	void ConfigureInterrupts(void);
	void ConfigureTimer(void);
	void ConfigureADC(void);
	void ConfigureEPWM(void);
	void ConfigureDAC(void);
	void SetupADC(void);
	void SetupInterrupts(void);
	void LoadData(void);
	void CanSetup(void);
};

#define CDARR 8

#endif /* HARDWARE_H_ */
