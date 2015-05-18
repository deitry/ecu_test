/*
 * transient.cpp
 *
 *  Created on: 12.09.2014
 *      Author: ����
 */

#include "../main.h"
#include "math.h"


using namespace EG;

int ledCnt = 0;
int canStampCnt = 0;

/**
 * ���������� �����. �� ����� �������� ����� ������ ����������, �.�. ��������� ����������
 * ��������� ������� ����������...
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Transient()
{
	if (manLed)
	{
		cpldLedToggle(LED_GREEN);

#ifdef IS_DOCKED
		ledCnt++;
		if (ledCnt == 200)
		{
			GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
			ledCnt = 0;
		}
#endif
	}

	if (canTime)
	{
		canStampCnt++;
		if (canStampCnt == 1000)
		{
			canStampCnt = 0;
			tPid.P = EC_TPROG;
			engine->sendCanMsg(tPid);
		}
	}

	// ����������� ������ ������
	// - ��������� ����� �������
	this->setInjPhi();
	// - ��������� �������� ������
	this->ModeCalc();
	// ���������� - �������� ��������� �� CAN - ������� "������������ ���������"?

	if (manDur)
	{
		prog_time = static_cast<float>(progCnt)	/ S2US * TIM2_DIV;
		progCnt = 0;	// ���������� ������
	}

	return 0;
}

#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setQCTrans(void)
{
	if (fabs(err) > EG::dZone)
	{
		// ���-�����
		QCprev = QC;
		QC = kP*err + kI*errI + kD*errD;
	}
}
