/**
 * hardware.h
 *
 * �������� ����� EC_Hardware, ������������ ������� ������ � �����������: ���������� � ����.
 *
 *  Created on: 29.10.2014
 *      Author: ����
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "interrupts.h"
#include "../drv/diesel_drv.h"
#include "F28x_Project.h"	// Device Headerfile and Examples Include File


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
