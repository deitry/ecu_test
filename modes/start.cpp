/*
 * start.cpp
 *
 *  Created on: 12.09.2014
 *      Author: ����
 */

#include "../main.h"
#include "math.h"


using namespace EG;

/**
 * �������� �����������.
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Start()
{
	// - �� 30 ��/��� ������ �� ���������� - ����� ��������
	if (nR._val < 30)
	{
		QC = 0;
		g_step2Us = 0;
		return 0;
	}

	// - �� 150 ��/��� - ������ 500 ��3/���� - �� ��������� ������� �������
	if (nR._val < 150)
	{
		// - ��������� ����� �������
		this->setInjPhi();
		// - ��������� �������� ������
		this->ModeCalc();
	}
	else
	{
		// - ������������ �� 300 ��/��� - ����� ������ ��������� ������� �������
		pedal = 300;
		// - ��������� �� ���������� �����
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
		// ���-�����
		QC1 = kP*err + kI*errI + kD*errD;
		if (QC1 > QCmax) QC1 = QCmax;
		if (QC1 < QCmin) QC1 = QCmin;
		QC = QC1;

		if (!manQC)
			g_step2Us = QCtoUS(QC);

		// TODO : ����������� �� Tcool - ������� ��������� �����

		//setInjector(g_step1Us, g_step2Us, g_duty1, g_duty2);
		// ���, ���� ����������� ��������

	}
}
