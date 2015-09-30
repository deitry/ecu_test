/*
 * ecu_global_defines.h
 *
 *  Created on: 11.02.2015
 *      Author: ����
 */

#ifndef ECU_GLOBAL_DEFINES_H_
#define ECU_GLOBAL_DEFINES_H_

typedef bool _Bool;

#include "F2837xD_Cla_typedefs.h"
#include "data/field.h"
#include "drv/can_if.h"
#include "can_par_id.h"

#define HALL_CRANKSHAFT		1				// ����� ��� ������������. 0 - �����������������, 1 - ���������� (������)
#define HMLTP				(HALL_CRANKSHAFT ? 1 : 2)	// Hall MuLTiPlier
#define HALL_POLARITY		0				// 1 - ���������� �� �������� �������, 0 - �� ������ �������

#define DIESEL_N_CYL		1			// ���������� ���������
#define DIESEL_N_CYL_MAX	12			// ����������� ��������� ���������� ��������� ��� ������ ������ �����
#define DIESEL_D_ZONE		0.5			// ���� ������������������ ���������� - ����� ������� ����������
#define DIESEL_Z_CUT		1			// ���������� ���������� �����
#define DIESEL_Z_ALL		120			// ����� ���������� ����� � ������ ����������
#define DIESEL_Z_PHI		(360/DIESEL_Z_ALL)		// ���� �� ������ ���� �� �������
#define DIESEL_Z_MAX		(DIESEL_Z_ALL-DIESEL_Z_CUT) 	// ���������� �������������� �����
#define DIESEL_PHI_MAX		(720/HMLTP)		// ���� �������� �� ���� (��� �������)
#define DIESEL_T_SETUP		3000		// ����� �������� (� ������) ����� ������������� setInjector
#define DPM					DIESEL_PHI_MAX	// ����������

#define FDBK_BUF			50				// ������ ������ ��� �������� �����
#define FDBK_C1				20
#define FDBK_C2				10
#define FDBK_C3				(FDBK_BUF-FDBK_C1-FDBK_C2)
#define PARIDMAX 			42				// ���������� ���������� ��� ������ �� CAN
#define SENSCNT				8				// ���������� ������� ��� ���������� ������������ ��������

#define	 	TIMER_FREQ		200		//Specified in MHz
#define	 	TIM1_DIV		100		// ��������, ������������ ������� ����������.
									// 100 = 100 ���, 1 = 1���
#define	 	TIM2_DIV		10		// ��������, ������������ ������� ����������.
#define		TIMER_PER		1		//Specified in microseconds
#define		TIMER1_PER		4		//Specified in microseconds
#define 	S2US			1e6		// ������� � ������������
#define 	PI				(3.1416)

//#define 	ERRI_MAX		0.0032


union CAN_DATA_VAL {
	double f;
	unsigned long int i;
};

struct CAN_DATA_FIELDS {                      // bits description
    Uint8 PARID;
    Uint8 R1;
    Uint8 R2;
    Uint8 R3;
    CAN_DATA_VAL val;
};

union CAN_DATA {
    Uint16 all[6];
    CAN_DATA_FIELDS f;
};

Uint16 cylToCode(int nCyl);
Uint16 cylToCodeX(int nCyl);

struct ECU_State
{
	unsigned work : 1;		// ���� �������� ������ / �� �������� � ����� � �������
	unsigned hall : 1;		// �������� �� ������� � ������� �����
	unsigned warning : 1; 	// ���� �� ������������� ���������� ������ � �����������
	unsigned error : 1;		// ���� �� ������������� ���������� ����� �� �����������
};

// �������� � �������������� �������������
class RestrictedValue
{
public:
	float _val;
	bool _restricted;
	float _min;
	float _max;
	// �������� �������������� �������?
	// �������������� ������� ����� ���������� �������� ��������, � ������� ����������
	// ������������, �� ��������� � ������� ������������� �������. ��� ������ ��
	// �������������� ������� ����� ����� ����������

public:
	RestrictedValue(float value, bool restricted, float minimum, float maximum);

	int Check();	// �������� - �� �� ������ (1) ��� �� ����� (0)

	/*float GetValue()
	{
		//if (!restricted)
			return value;

		if (value > max)	// *1.05 - ��������� 5 %
		{
			return max;
		}

		if (value < min)	// *1.05 - ��������� 5 %
		{
			return min;
		}
	}*/

	//void SetValue(float nvalue)
	//{

	//}
};

/**
 * ������������ ��� ��� ���������� ���������� // ��� ����������� ������� �������
 *
 * EG //I = ECU_GLOBAL //_INJECTION
 */
namespace EG //I
{
	extern Uint32 ecu_state;				// �������� ���, ��������������� ������ �����


	extern int cylChannel[DIESEL_N_CYL_MAX];	// ������ ������������� ����� ������������������ �������� ��������� � �������� �������
	extern float injPhi[DIESEL_N_CYL];		// ����, ������������ ������ �������
	extern int injZ[DIESEL_N_CYL];			// ���, ������� � ��������
	extern int dTime[DIESEL_N_CYL];			// ����� � ���, ������� ������������� �� ����
	extern Uint32 injCnt[DIESEL_N_CYL];		// �������, ������������� � �������� ���������� ������� ����� ��� �������
	extern Uint8 injSw[DIESEL_N_CYL];		// ����, ������������ ������������� ������� � ��������������� �������
	extern Uint32 injN[DIESEL_N_CYL];		// ����� ��������� ��������
	//extern bool injection;

	extern Uint8 g_duty1;					// ���������� ������������ �������
	extern Uint8 g_duty2;					// ���������� ������������� �������
	extern Uint16 g_step1Us;				// ����������������� ������������ �������, ���
	extern Uint16 g_step2Us;				// ����������������� ������������� �������, ���
	extern Uint16 g_step3Us;				// ���������� ������� ��� ��������� �����������
											// �� ���� ��� �������� � ������������ �� ������������
	extern Uint16 g_wakeup_High;			// ����������������� "�������������" �������� ��� ��������
	extern Uint16 g_wakeup_Period;			// ������ "�������������" �������� ��� ��������
//}

/**
 * ������ � ������� �������
 */
//namespace EGN
//{
	extern float err, errI, errD;			// ������������ ������ ��� ������� ������ �� ����
	extern float errImax, errImax1;					// ����������� ���������
	extern float kP, kI, kD;				// ������������ ����
	extern RestrictedValue nR;						// ������� �������, ��/���
	extern float nR_max;					// ������� ����������� ��� ��
	extern float nR_min;					// ������ ����������� ��� ��
	extern float nU;						// ������� �������, ��/���
	extern float omegaR;					// ������� �������, ���/�
	extern float QC, QCmin, QCmax, QCsp;			// ������, ��/����, ������ � ������� �����������
	extern float QCadop, alphaDop;			// ��� ����������� �� �����

	extern float muN;						// ����������� ����� ������ ������� ��������
											// (�� ����� - ���� ��������� �������)

	//extern float Pinj;						// �������� �������
	extern RestrictedValue Pinj;
	extern float kQc;						// ���������� ����������� ������-�������-�������
	extern float pedal;						// ��������� ������ (? � ������ �������� � ������� ��������?)
	extern int finTime;						// ����� �������� �� ������ �� ������ EC_Finish ����� ���������� 400 ��/���

	extern float Pk;						// �������� ������� (������ �� ������������)
	extern float Tv;						// ����������� ������� �� �����

	extern float anVMT;

//}

/**
 * ����� ��� ���������� ���������� ��������� ���������
 */
//namespace EGM
//{
	extern int manQC;						// ������ ����� ������� ������
#define EG_MANQC_AUTO	0					// 0 - ������ �������������� ������������� �� ���-������
#define EG_MANQC_TIME	1					// 1 - ������ ����� �� ��������������, ������������ ����� g_step2Us
#define EG_MANQC_ANGLE	2					// 2 - ����������������� ������ ������������ �� ���� injAngle
#define EG_MANQC_QC		3					// 3 - ����������������� ������ �������������� ������ �� �������� QC
											//		� ������� �������� QCtoUS()

	//extern int manQCt;						// ������� ������ � ���
	extern int manQCRelay;						// ������� ������ � ���
	extern int manPhi;						// ������ ����� ������� ������� ������ ������� � ��������
	extern int manOUVT;						// ������ ����� ������� ���� ���������� ������� �������
											// (������� ���� �������������� �������������)
	extern float injOuvt;					// ���� ���������� ������� ������� ��� ������� � ������ ������
	extern int manMode;						// ������ ����� ������� ������ ������ �����
											// (������ - �� ������� �� ����������� �������)
	extern int manPed;						// ������ ����� ������� �������� ������
	extern int manInj;						// ������ ����� �������
	extern int manN;						// ������ ������ ��������� �� �������� � ������ ������ � ���
											// (���� ���� - �������� ����������� ������)
	extern int injOnce;						// ���� ��� ������������ �������
											// (�� ������������, ���� manN > 0)
	extern int injCyl;						// �������, � ������� �������������� ������ � ������ ������
	extern int manCnt;						// ������� ��� ������� ������ �������
	//extern int manAngle;					// ��������� ������ ������� ����������������� ������� � ��������
											// (��� ������ ���������� �������� manQC)
	extern float injAngle;					// ����������������� ������� � ��������

	extern int canSend;						// �������� ���������� �� CAN
	extern int canTime;						// �������� ��������� �����
	extern int manFdbk;						// ����� �������� ����� �� ����
	extern int manDur;						// ����� �������� ����� �� ����
	extern int manLed;						// ������� ������������
	extern int manSens;						// ��������� �������� (Pk, Tv...)
	extern int pedStep;						// ��� ��������� ������ - ����������� �� ���� ��������
//}

/**
 * ���������� ��� ������ � ���������
 */
//namespace EGS
//{
	// ���������� ��� ������ ��������
	extern float32 val;						// ��������
	extern Uint16 sens;						// ��� �������
	extern Uint8 chan;						// �����

	extern float delta_time;				// ������� ������� ����� ������� � ���������� �����
	extern float delta_time1;				// ������� �� ������� ����� �������� ������� ������������� � ���������� �����
	extern float int_time;
	extern float int_time1;
	extern Uint32 ttim;
	extern float prog_time;

	extern int inOut;						// ������������� ����� �������� �������� � ������� �������� �� ��������������� ����������
	extern float32 valX[SENSCNT];
	extern Uint16 sensX[SENSCNT];
	extern Uint8 chanX[SENSCNT];

	extern float32 valP;	// Pk
	extern Uint16 sensP;
	extern Uint8 chanP;
	extern float Pk_koeff;
	extern float32 valT;	// Tv
	extern Uint16 sensT;	// ����������� ������� �� ������
	extern Uint8 chanT;
	extern float Tv_koeff;

	extern float nU0;						// �������� ������� (? ������� �������� ��������� ����?)

	extern float dZone;						// ���� ������������������ ����
	extern float dZone1;					// ���� ����, � �������� ������� ���������������� ��������� �� 0.6
	extern float PIDstab;					// �����������, �� ������� ����������� �-�� ���� � �������� dZone1
	extern float dOmega;					// ���������� ������� ��������
	//extern float errDMax;					// (�� ���.) ������� ����������� ����������� �� ������
	extern float errRelayMax;				// ����������� ��������� ������ (� ������������� ������� - ���� ������� ��������), ����� �-�� �� ���������� ������
	extern float QCprev;					// (�� ���.) ���������� �������� ������

	extern float32 fdbkBuf[FDBK_BUF];		// ����� ��� �������� ����� � ����������
	extern int fdbkTBuf[FDBK_BUF];		// ����� ��� �������� ����� � ����������

	extern int progCnt;
	extern int fdbkTCnt;
	extern int getFdbk;		// ������� �������� �����

	// ��������������� ���������� ��� ������ �������� �����
	extern Uint16 n1;
	extern Uint16 n2;
	extern Uint16 n3;
	extern Uint16 d1;
	extern Uint16 d2;
	extern Uint16 d3;
//}

/*
 * ���������� ��� ������ � CAN
 */
//namespace EGC
//{
extern tCANMsgObject canReceiveMessage;	// ������, ������������ ���������� �� CAN ���������
	extern CAN_DATA canTransmitMessage;	// ������, ������������ ���������� �� CAN ���������
	extern Uint8 canReceiveData[8];			// ������ � ���������� ���������
	extern PAR_ID_BYTES canTransmitId[PARIDMAX];			// TODO : ���������� �� ����������� ������

	extern CAN_DATA can_data;
	extern int cntCanTransmit;
	extern int cntCanTransmit2;
	extern CANListElement* elCanTransmit;	// ������� ������� ������������� ������
	extern PAR_ID_BYTES curCanId;
	extern int canLock;
	extern int canLockM;
	extern int fdbkLock;		// ������������ ������ � ����� �� ������ ��������
	extern int fdbkAll;			// ������������ �������� ������ �� ����� ������
	extern Uint8 fdbkCyl;

	// ��������� ��� ����������� ��������� �����
	extern PAR_ID_BYTES tPid;
	extern Uint8 zCnt;		// ������� ������
	extern Uint16 nCnt;		// ������� ��������
	extern Uint32 timeCnt;		// ������� �������

	extern PAR_ID_BYTES singlePID;	// ������������� ���������� ��� ���������� �������

	extern int tmpi;
	extern PAR_ID_BYTES tparid;
}

/**
 * ������������ ���, ������������ ������� � �������
 *
 * EGD = ECU_GLOBAL_DATA
 */
namespace EGD
{
	extern const double OUVTdata[15][15];	// ���� ���������� ������� ������� � ���-�� �� ������� � ������
	extern const double SpCharData[7];		// x0400 d100 max7
	extern const double NDSteadyData[3];	// ������� ��������� ���� � ����������� �� ����
	extern const double QCStartData[3];		// �������� �������� ������ � ����������� �� ����

	extern FMField2* OUVT;					// ���� ���������� ������� �������
	extern FMField1* SpChar;				// ���������� ��������������
											// (���� ������ � ����������� �� �������)
	extern FMField1* NDSteady;
	extern FMField1* QCStart;
}


#endif /* ECU_GLOBAL_DEFINES_H_ */



