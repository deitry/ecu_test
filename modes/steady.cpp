/*
 * steady.cpp
 *
 *  Created on: 12.09.2014
 *      Author: ����
 */

#include "../main.h"

using namespace EG;

/**
 * ������������ �����. ���������������� ����� �������������� ������ ��� �����������
 * ��������� ����, � ���� ������� ����� ��������� ��������������...
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Steady()
{
	// TODO : pedal = 400, ������� ����� ���� ���������
	nU = 400;	// TODO : ������� ��������� ����

	for(;;)
	{
		if (manLed)
		{
			cpldLedToggle(LED_GREEN);
		}

		// �������� ��������� ������� ����������
		if (!this->ControlCheck())
		{
			// ��������� ������ ������ - ���������� ������� � �.�.
			if (mode != EC_Steady) return 0;
		}

		// - ��������� ����� �������
		setInjPhi();
		// - ��������� �������� ������
		this->ModeCalc();
		this->Monitoring();
	}
}


