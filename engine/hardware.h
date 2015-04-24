/**
 * hardware.h
 *
 * �������� ����� EC_Hardware, ������������ ������� ������ � �����������: ���������� � ����.
 *
 * ��������, �� ����� ������ ������, ���� �����������, ����� ���������.
 *
 *  Created on: 29.10.2014
 *      Author: ����
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

//#include "ecu_global_defines.h"
#include "interrupts.h"
#include "../drv/diesel_drv.h"
#include "F28x_Project.h"	// Device Headerfile and Examples Include File

//#define DELAY (CPU_RATE/1000*6*510)  //Qual period at 6 samples


/**
 * �� ����, ���� ����� ���������� ��� ��������� ������ ������ � �������������.
 * ����� ����� ����� ������ ���� �� ��������� ���������. (?)
 */
class EC_Hardware {
public:
	EC_Hardware();			// ������� �����������
	void Initialise(void);	// ������ ��������� ���������

protected:
	 // ��������� ��� ��������� ����������.
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
