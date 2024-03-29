/*
 * engine.cpp
 *
 *  Created on: 15.09.2014
 *      Author: ����
 */

#include "engine.h"
#include "math.h"

using namespace EG;


/**
 * ������� �����������.
 */
//#pragma CODE_SECTION("ramfuncs")
EC_Engine::EC_Engine()
{
	// ��������� � ����� ����
	TenvMin = 300;
	TcoolMin = 300;

	this->tCool = 0;

	this->mode = EC_Transient;
	// ������������� ��������� ��������
	hard = new EC_Hardware();
	hard->Initialise();

	devices = new EC_DeviceList();
	devices->addDevice(EC_S_D_PEDAL, (EC_Device*) new EC_Sensor(TEMPERATURE_SENSOR, TEMP_SENS_CHANNEL_2, 0, 10000, 2.77 * 5 / HMLTP, 0, 1));
	devices->addDevice(EC_S_D_PK, (EC_Device*) new EC_Sensor(CURRENT_SENSOR, TEMP_SENS_CHANNEL_8, 100, 500, 1640, 0, 1));
	devices->addDevice(EC_S_D_TV, (EC_Device*) new EC_Sensor(CURRENT_SENSOR, CURR_SENS_CHANNEL_7, 220, 500, 1464, 0, 1));
	devices->addDevice(EC_S_D_PINJ, (EC_Device*) new EC_Sensor(CURRENT_SENSOR, CURR_SENS_CHANNEL_3, -1, 16, 63.69426752, -4.292993631, 1));
}

//#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setQCrelay()
{
	if (err < -errRelayMax)
	{
		QC = QCmin;
	} else {
		setQCTrans();
	}
}

void EC_Engine::clearPID(void)
{
	nU = nR._val;	// ���������� ������� �� ������� ��������� ������� ��������
		// �������� pedal ������� �������, ������� ����� �������� ��������� ���� ������
	errI = (QC - kP*err - kD*errD)/kI;
}

/**
 * ���������� ������ ������.
 * �� ������ ������ ��� ���� ��������������� ����� ���� �������� � ���������
 * ������������ ����������� �������� ������������.
 *
 * ================================================================================================
 *
 * �� ���� � setInjector() ����� ��������� ����������� �������� g_step2Us,
 * ������� ��� ������ ������ ������������ �������� � �����������.
 * ������� �������� QC � ��� ������ �������������� � ����� ����� ModeCalc(),
 * ����� ����������� �.�. �������� ��������������� ����� ����������, � � ���������� setQCxxx()
 * ������ �������������� ������ ������ �������� �������������� ������� ��� �����������
 *
 * ================================================================================================
 *
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::ModeCalc()
{
	if (!manQC)
	{
		// ������������ ���������� ������� � ����������� �� ������.
		switch (this->mode)
		{
		// ��������� ���������� �������
		case EC_Start:
			QC = QCstart;
			g_step2Us = QCtoUS(QC);
			//QC = EGD::QCStart->get(Tcool());
			break;

		// �������� ��� - ����������� - ���
		case EC_Steady:
			// ������� � ����������� �� ����������� ����������� ��������
			nU = EGD::NDSteady->get(Tcool()) / HMLTP;
			setQCTrans();
			break;

		// ������� ����� - ��� ������������ ������� ��������
		case EC_Transient:
			// - ���������� ������� - ��������� ������
			if (!manPed)
				pedal = Pedal();
			// ��������� ������� � ������������ � ������ ������ ���������� � tim2

			// �������� �����, ���� ��������. �� ���������� ����� ������ ��� ������������� ���������
			// ���� ���������� �������� (errRelayMax)
			//if (manQCRelay)
			//{
			//	setQCrelay();
			//} else {
				setQCTrans();
				QCRestriction();
				// ��������� �������� ��� �������
				g_step2Us = QCtoUS(QC);
			//}
			break;

		// ������� ����� - ��� ������������ ������� ��������
		case EC_Failure:
			// - ���������� ������� - ��������� ������
			if (!manPed)
				pedal = Pedal();

			// ����������� ����� ������
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			setQCTrans();
			break;

		// ���������� ����� - �� ������
		case EC_Automotive:
			QC = Pedal()/100 * QCmax; // � �������������, ��� ������ �� 0 �� 100 ...
			// QCmax
			break;

		case EC_Finish:
			pedal = 400/HMLTP;

			// ����������� ����� ������
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			setQCTrans();
			break;
		case EC_MaxDyn:
			// - ���������� ������� - ��������� ������
			if (!manPed)
				pedal = Pedal();

			// ����������� ����� ������
			if ((muN > 0) && (muN < 1) && (fabs(nU - pedal) > 1) )
			{
				nU += muN*(pedal)*fabs(pedal-nU)/(pedal-nU);
			} else {
				nU = pedal;
			}

			QCmax = 10000;	// �������������� g_step2UsMax
				// ���������� QCmax, ����� ��� ���������� �� ����������� ���������
				// � interrupts.cpp
			setQCMaxDyn();	// ������������� ��������������� g_step2Us
			if (g_step2Us > QCmax) g_step2Us = QCmax;
			break;
		case EC_Standby:
		default:
			break;
		}
	}
	else
	{
		// ������ �����
		switch (manQC)
		{
		case EG_MANQC_ANGLE:
			g_step2Us = angleToTime(injAngle);
			QC = (injAngle - 3) * 120;
			break;
		case EG_MANQC_QC:
			g_step2Us = QCtoUS(QC);
			break;
		case EG_MANQC_TIME:
		default:
			// ������ �� ������, �������� g_step2Us ������� ����� ����� ����
			// ������ ���� ��� �� �������� ����� can
			injAngle = timeToAngle(g_step2Us);
			QC = (injAngle - 3) * 120;
			break;
		}
	}

	// �������� �� ����������� ��������
	// ����� �� ������ �������� ��������� ����� ����� �������� - ����� ����� �� ������
	/*int flag = 1;
	for (int i = 0; i < DIESEL_N_CYL; i++)
	{
		if (injSw[i])
		{
			flag = 0;
			break;
		}
	}
	if (flag)
	{*/
		setInjector(g_step1Us, g_step2Us, g_duty1, g_duty2);
	//}

	// ��� ���� ���������� setInjector
	for (int i = 0; i < DIESEL_T_SETUP; i++)
		asm(" NOP ");

	// ����� ��� ��������� �������� �����
	n1 = (g_step1Us - g_step3Us/2)/TIM2_DIV;
	n2 = g_step3Us/TIM2_DIV;
	n3 = g_step2Us/TIM2_DIV;
	d1 = n1/FDBK_C1;
	d2 = n2/FDBK_C2;
	d3 = n3/FDBK_C3;

	return 0;
}

/**
 * ������� �������� �������� ������ � ����������������� ������������� �������� � ���
 */
#pragma CODE_SECTION("ramfuncs")
Uint16 EC_Engine::QCtoUS(float qc)
{
	//int tmp = qc/nR._val*1e9*kQc/HMLTP; // ������� �� ��/���� � ���
	injAngle = QC/120 + 3;	// ������� �� ��3/���� � �������
	Uint16 step2 = angleToTime(injAngle);
	if (step2 > 0)
	{
		return step2;
	}
	else return 0;
}

/**
 * ��������� ����� ��������. �������������, ��������� �������� � ���� ���������� �������
 */
#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setInjPhi(void)
{
	for (int ii = 0; ii < DIESEL_N_CYL; ii++)
	{
		if (!EG::manPhi)
		{
			// ���� ������� � ������ ���� ���������� � ��������� �� ����������� �������
			injPhi[ii] = ii * DIESEL_PHI_MAX/DIESEL_N_CYL
					 - EG::g_step1Us*EG::nR._val/S2US/60 * DIESEL_PHI_MAX;	//  ? ����� � ��� ������� ��������?

			// ���� ����������
			if (!EG::manOUVT)
			{
				if (this->mode == EC_Start)
				{
					// �� ������ ������������� ���� ���������� 5 ��������
					injOuvt = -5 / HMLTP;
				}
				else
				{
					injOuvt = EGD::OUVT->get(EG::QC, EG::nR._val*HMLTP) / HMLTP;
				}
			}
			injPhi[ii] += injOuvt + anVMT;
		}


		// �������� �������� ���� ������� � ��������� ���������
		while (injPhi[ii] < 1)
		{
			injPhi[ii] += DIESEL_PHI_MAX;
		}
		while (injPhi[ii] >= DIESEL_PHI_MAX+1)
		{
			injPhi[ii] -= DIESEL_PHI_MAX;
		}
		// ������� ����� ����
		int zii = injPhi[ii] / DIESEL_Z_PHI;

		// ������ ���, ����� ����� �� ������ ���� (��� �� ������ �����������)
		// �� ������� ������� �� ���� ����� ���� � ����� ����.
		if (!((int)(injPhi[ii]) % DIESEL_Z_PHI))
		{
			zii--;
		}

		// ��������� �� ������� � ���������� �����
		if (zii >= DIESEL_Z_ALL*2-DIESEL_Z_CUT-1)
		{
			zii = DIESEL_Z_ALL*2-DIESEL_Z_CUT-1;
		}
		else if ((zii >= DIESEL_Z_MAX) && (zii < DIESEL_Z_ALL))
		{
			zii = DIESEL_Z_MAX-1;
		}

		// ���������� ������������ ����������� ��������
		injZ[ii] = zii;
		// ������� �������
		dTime[ii] = (injPhi[ii] - injZ[ii]*DIESEL_Z_PHI)*3.14/180/omegaR*S2US;	// ��c
	}
}

/**
 * �������� ������� � �����
 */
/*int EC_Engine::SysCheck()
{
	return this->list->Check();
}*/

/**
 * �������� ������� ���������� - �� �� ��������, ��� �� ������ ��������� � �.�.
 * ������ - ������������ �������
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::ControlCheck()
{
	if (0) //!manMode)
	{
		// �������� ������ ���������� �� ������������ �������
		switch (Key())
		{
		case 1:
			mode = EC_Start; break;
		case 2:
			mode = EC_Steady; break;
		case 3:
			mode = EC_Transient; break;
		case 4:
			mode = EC_Automotive; break;
		case 5:
			mode = EC_Econom; break;
		case 6:
			mode = EC_MaxDyn; break;
		case 7:
			mode = EC_Finish; break;
		case 8:
			mode = EC_Failure; break;
		case 9:
			mode = EC_Crash; break;
		default:
			mode = EC_Failure;
		}
	}

	return 0;
}



/**
 * ������� ���� � ����������������� ������������� �������� � ��� - ���������� ��� ������� ������ � �����
 */
#pragma CODE_SECTION("ramfuncs")
float angleToTime(float angle)
{
	return angle/omegaR* PI/180 *S2US;
}

/**
 * ������� ����������������� � ����.
 * ������ �� ������������.
 */
#pragma CODE_SECTION("ramfuncs")
float timeToAngle(float time)
{
	return omegaR*time;
}

#pragma CODE_SECTION("ramfuncs")
void EC_Engine::QCRestriction(void)
{
	// ����������� ������
	// 1. ���������� ��������������
	QCsp = EGD::SpChar->get(nR._val*HMLTP);
	// 2. ���������������
	if (manQCalpha)
	{
		QCadop = Pk*18.29/(alphaDop*287.*Tv*14.3) / 800 * 1e9;
		// - ��� QCmax �������� �����������
		QCmax = ((QCsp < QCadop) ? QCsp : QCadop);
	}
	else
	{
		QCmax = QCsp;
	}

	// ���������� �����������
	if (QC > QCmax)
	{
		QC = QCmax;
	}
	if (QC < QCmin)
	{
		QC = QCmin;
	}
}

#pragma CODE_SECTION("ramfuncs")
void EC_Engine::setQCMaxDyn(void)
{
	if (fabs(err) > EG::dZone)
	{
		// ���-�����
		g_step2Us = kP*err + kI*errI + kD*errD;
	}
}
