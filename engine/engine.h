/*
 * engine.h
 *
 *  Created on: 15.09.2014
 *      Author: ����
 */

#ifndef ENGINE_H_
#define ENGINE_H_

//#include "ecu_global_defines.h"
#include "devices.h"
#include "hardware.h"
#include "../can_par_id.h"

// ������ �������
#define EC_MAX_MODE 10
enum eng_mod { 	EC_Start = 1,		// ����
				EC_Steady,			// �������� ���
				EC_Transient,		// ������-�������
				EC_Automotive,		// ������-������
				EC_Econom,			// �������������
				EC_MaxDyn,			// ������������ ��������
				EC_Finish,			// �������
				EC_Failure,			// ������
				EC_Crash,			// ����
				EC_End};			// �����

// ��������������� �������.
void sendCanMsgX(CAN_DATA* data);
void sendCanMsg(int id, Uint32* val);
void sendCanMsg(int id, float* val);

float timeToAngle(float time);
float angleToTime(float angle);


// ��������� ���������
class EC_Engine
{
public:
	EC_Engine();		// ������� �����������

	int Run();

protected:
	// ������ ������� �������. ��� ��������� ��� ��������������� ���������.
	// ������ ����� ���������� ��������� _�����������_ ������������������ ��������.
	// ���� ������ ���� ������ ���� - ���������� (Run())
	int Initialise();	// ������������� ������� ���������� - ����� �� ��������?
	int Start();		// ����
	int Finish();		// ���������� ������
	int Steady();		// ������������ ����� - �������� ���
	int Transient();	// ������� �� ������ �����
	int Automotive();	// ������� �� ������ �����
	int Failure();		// ��������� �����
	int Finalise();		// ����������� ������� ���������� - ����� �� ��������?

	//int SysCheck();	// �������� ��������� ������� � ����� � ��������� ���������
	int ControlCheck();	// �������� ��������� ��������� ������� ����������
	int ModeCalc();		// ���������� ������ ������

	int Monitoring(void);	// �������� ���� �� SysCheck(), �� ����������� ���������, �� �������.

	eng_mod mode;		// �����

protected:

	// - - - ��������� ������
	static Uint16 QCtoUS(float qc);
	void setInjPhi(void);

	void setQCTrans(void);
	void setQCAuto(void);
	void setQCSteady(void);
	void setQCrelay(void);

	// - - - ������� � ���������� ��������� � ���� ������� - ������� � ��������� ������� ������
	float WD();		// ������� ��������
	float Tenv();	// ����������� ��������� �������
	float Tcool();	// ����������� ����������� ��������
	float Pedal();	// ��������� ������: 0-100
	int Key();		// ��������� ��������� �����

	// - - - ��������������� ��������� ������ ��������� ���������?
	float TenvMin;	// ���������� ���������� �������� � ��� ��, ��� ������� �� ����� �������
	float TcoolMin;	// ���������� �������� ����, ��� ������� �� ����� ������
	float tCool;

	// ���������� "�����" ���������, �� ������� ����� ����
	// �� �������� ��������� �������� � � ����������� �� ���� ��������������
	// ��������� ����������. ����� � ����� �������� � ���� ���������� ������, �������
	// ����� �������� �� �������������� ����� � ���������� �����.
	EC_DeviceList* devices;

	/**
	 * ��� ��������� ������ �� �������������� � �����������
	 */
	EC_Hardware* hard;


public:
	int sendCanMsg(PAR_ID_BYTES id);
	void recieveCanMsg(tCANMsgObject* canSendMessage);
};



#endif /* ENGINE_H_ */
