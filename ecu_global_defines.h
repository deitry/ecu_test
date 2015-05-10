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
#include "drv/diesel_drv.h"
#include "drv/can_if.h"
#include "can_par_id.h"
//#include "engine/engine.h"

#define HALL_CRANKSHAFT		0				// ����� ��� ������������. 0 - �����������������, 1 - ���������� (������)
#define HMLTP	(HALL_CRANKSHAFT ? 1 : 2)	// Hall MuLTiPlier
#define HALL_POLARITY		0				// 1 - ���������� �� �������� �������, 0 - �� ������ �������

#define DIESEL_N_CYL	1			// ���������� ���������
#define DIESEL_D_ZONE	0.5			// ���� ������������������ ���������� - ����� ������� ����������
#define DIESEL_Z_CUT	1			// ���������� ���������� �����
#define DIESEL_Z_ALL	45			// ����� ���������� ����� � ������ ����������
#define DIESEL_Z_PHI	(360/DIESEL_Z_ALL)		// ���� �� ������ ���� �� �������
#define DIESEL_Z_MAX	(DIESEL_Z_ALL-DIESEL_Z_CUT) 	// ���������� �������������� �����
#define DIESEL_PHI_MAX	(720/HMLTP)		// ���� �������� �� ���� (��� �������)
#define DIESEL_T_SETUP	3000		// ����� �������� (� ������) ����� ������������� setInjector
#define DPM				DIESEL_PHI_MAX	// ����������

#define FDBK_BUF		50				// ������ ������ ��� �������� �����
#define FDBK_C1			20
#define FDBK_C2			10
#define FDBK_C3			(FDBK_BUF-FDBK_C1-FDBK_C2)
#define PARIDMAX 		41				// ���������� ���������� ��� ������ �� CAN
#define SENSCNT			8				// ���������� ������� ��� ���������� ������������ ��������

#define	 	TIMER_FREQ		200		//Specified in MHz
#define	 	TIM1_DIV		100		// ��������, ������������ ������� ����������.
									// 100 = 100 ���, 1 = 1���
#define	 	TIM2_DIV		10		// ��������, ������������ ������� ����������.
#define		TIMER_PER		1		//Specified in microseconds
#define		TIMER1_PER		4		//Specified in microseconds
#define 	S2US			1e6		// ������� � ������������
#define 	PI				(3.1416)


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

/**
 * ������������ ��� ��� ���� ���������� ����������
 *
 * EG = ECU_GLOBAL
 */
namespace EG
{
	extern float injPhi[DIESEL_N_CYL];		// ����, ������������ ������ �������
	extern int injZ[DIESEL_N_CYL];			// ���, ������� � ��������
	extern int dTime[DIESEL_N_CYL];			// ����� � ���, ������� ������������� �� ����
	extern Uint32 injCnt[DIESEL_N_CYL];		// �������, ������������� � �������� ���������� ������� ����� ��� �������
	extern Uint8 injSw[DIESEL_N_CYL];		// ����, ������������ ������������� ������� � ��������������� �������
	extern Uint32 injN[DIESEL_N_CYL];		// ����� ��������� ��������
	//extern bool injection;

	extern float err, errI, errD;			// ������������ ������ ��� ������� ������ �� ����
	extern float kP, kI, kD;				// ������������ ����
	extern float nR;						// ������� �������, ��/���
	extern float nU;						// ������� �������, ��/���
	extern float omegaR;					// ������� �������, ���/�
	extern float QC, QCmin, QCmax;			// ������, ��/����, ������ � ������� �����������
	extern float QCadop, alphaDop;			// ��� ����������� �� �����
	extern volatile Uint8 g_duty1;			// ���������� ������������ �������
	extern volatile Uint8 g_duty2;			// ���������� ������������� �������
	extern volatile Uint16 g_step1Us;		// ����������������� ������������ �������, ���
	extern volatile Uint16 g_step2Us;		// ����������������� ������������� �������, ���
	extern volatile Uint16 g_step3Us;		// ���������� ������� ��� ��������� �����������
											// �� ���� ��� �������� � ������������ �� ������������

	extern float muN;						// ����������� ����� ������ ������� ��������
											// (�� ����� - ���� ��������� �������)


	extern float mag;						// ��������� ����������� �����������
	extern int magi;
	extern float kQc;						// ���������� ����������� ������-�������-�������

	extern int manQC;						// ������ ����� ������� ������
	extern int manQCt;						// ������� ������ � ���
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
	extern int manAngle;					// ��������� ������ ������� ����������������� ������� � ��������
											// (��� ������ ���������� �������� manQC)
	extern float injAngle;					// ����������������� ������� � ��������

	extern int canSend;						// �������� ���������� �� CAN
	extern int canTime;						// �������� ��������� �����
	extern int manFdbk;						// ����� �������� ����� �� ����
	extern int manDur;						// ����� �������� ����� �� ����
	extern int manLed;						// ������� ������������
	extern int manLed1;						// ������� ������������
	extern int manLed2;						// ������� ������������
	extern int manSens;						// ��������� �������� (Pk, Tv...)
	extern int pedStep;						// ��� ��������� ������ - ����������� �� ���� ��������

	extern float pedal;						// ��������� ������ (? � ������ �������� � ������� ��������?)
	extern int finTime;						// ����� �������� �� ������ �� ������ EC_Finish ����� ���������� 400 ��/���

	extern float Pk;						// �������� ������� (������ �� ������������)
	extern float Tv;						// ����������� ������� �� �����

	extern float anVMT;

	// ���������� ��� ������ ��������
	extern float32 val;						// ��������
	extern Uint16 sens;						// ��� �������
	extern Uint8 chan;						// �����

	extern float delta_time;
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
	extern float dOmega;					// ���������� ������� ��������
	//extern float errDMax;					// (�� ���.) ������� ����������� ����������� �� ������
	extern float errRelayMax;				// ����������� ��������� ������ (� ������������� ������� - ���� ������� ��������), ����� �-�� �� ���������� ������
	extern float QCprev;					// (�� ���.) ���������� �������� ������

	extern tCANMsgObject canReceiveMessage;	// ������, ������������ ���������� �� CAN ���������
	extern CAN_DATA canTransmitMessage;	// ������, ������������ ���������� �� CAN ���������
	extern Uint8 canReceiveData[8];			// ������ � ���������� ���������

	extern float32 fdbkBuf[FDBK_BUF];		// ����� ��� �������� ����� � ����������
	extern int fdbkTBuf[FDBK_BUF];		// ����� ��� �������� ����� � ����������

	extern PAR_ID_BYTES canTransmitId[PARIDMAX];			// TODO : ���������� �� ����������� ������

	extern CAN_DATA can_data;
	extern int cntCanTransmit;
	extern int cntCanTransmit2;
	extern PAR_ID_BYTES curCanId;
	extern int canLock;
	extern int canLockM;
	extern int fdbkLock;		// ������������ ������ � ����� �� ������ ��������
	extern int fdbkAll;			// ������������ �������� ������ �� ����� ������
	extern Uint8 fdbkChan;

	// ��������� ��� ����������� ��������� �����
	extern PAR_ID_BYTES tPid;
	extern Uint8 zCnt;		// ������� ������
	extern Uint16 nCnt;		// ������� ��������
	extern Uint32 timeCnt;		// ������� �������

	extern int tmpi;
	extern PAR_ID_BYTES tparid;

	extern int progCnt;
	extern int fdbkTCnt;
	extern int getFdbk;		// ������� �������� �����
	//extern int

	extern Uint16 n1;
	extern Uint16 n2;
	extern Uint16 n3;
	extern Uint16 d1;
	extern Uint16 d2;
	extern Uint16 d3;
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



