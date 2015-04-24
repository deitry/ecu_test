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
 *
 * ������ �������� �������� ������ �� ���������.
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Start()
{
	nU = 400;
	for(;;)
		{
			if (manDur)
			{
				cpldLedToggle(LED_GREEN);
			}

			// ������� ������
			if (fabs(nU-nR) < 1)
			{
				this->mode = EC_Steady;
				return 0;
			}

			// �������� ��������� ������� ����������
			if (!this->ControlCheck())
			{
				// ��������� ������ ������ - ���������� ������� � �.�.
				//this->ModeCalc();
				if (mode != EC_Start)
					return 0;
			}

			// - ��������� ����� �������
			setInjPhi();
			// - ��������� �������� ������
			//setQCTrans();
			this->ModeCalc();
			this->Monitoring();

		}

/*
 * TODO :
 * 1 - �������� ���� �������
 * 2 - ������ � ������������ �� ������ QCmax(Tcool); ���� ���������� = -5
 * 3 - ��� n > 150 ��/��� ���� 1 ����������
 * 4 - ����� �� 400, ������� �� ����� ��������� ����
 */


	do
	{
		// ����������� ������� �����������
		/*if (this->SysCheck())
		{
			// ���� != 0 -> ������ ��������
			// *��������� �������������
		}*/
	} while (this->Key());	// ����� - *�������� ��������� ��������� �����

	// *����� �������� T���, T���
	// TODO : ������ ���� ������� ����� ������� � ��������� �������, ����� �� ������ �����
	// ����� boolean, ����� �� ����������� ������ � ������ ���� ������� ���������.
	if ((this->Tenv() < this->TenvMin) && (this->Tcool() < this->TcoolMin))
	{
		// *������� ���������
	}

	// *����������� �������� ������ ������� - �� �������

	// *��������� �������

	// *��������� ������ ������� ������� t (?)

	// *����������� ���������� ���������� �������, ������������ ����� ������ �������
	// (��� ������ ���������� �������)

	// *���� ���������
	for(;;)	// ���������� �� while?
	{
		// *���� ���������, ���� ������� ������� �������� = n_���� > n_������, ��� ������
		// *���� �� ����������� ���������� ����� ������ ������� ��������� �� ����������, ��
		// ���������
	}

	// *���� ���-�� �� ����������
	if (0)
	{
		// *������� - ���������� ������ �������, ��������� ������
		// *���������
		return 1;
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
