/*
 * can_par_id.h
 *
 * �������� ����������� ��������������� ��� �������� ���������� ����� CAN.
 *
 * ������� ���������� ��������� �������:
 * - �������� ������������� ���������� ��� �������,
 * - �������������� - ������ �������� �������� � ���� �������
 * (����������� ����� ���� �� ��� ��������, �.�. �� ����� ���������� ��������� �������)
 *
 * ����� �� ������� ����� �������� ������ ����������.
 * � �������� ����� ������ ����� ���� �������� 256 ���������������.
 *
 *  Created on: 18.03.2015
 *      Author: ����
 */

#ifndef CAN_PAR_ID_H_
#define CAN_PAR_ID_H_

struct PAR_ID_BYTES
{
	Uint8 P:8;
	Uint8 S:8;
	Uint8 S2:8;
	Uint8 S3:8;
};

// ������� ������ ��� ������� �� CAN
// �������� ��� ����������, ������ ��� �� ����� ������� ����������, � ����� ��������� ����
class CANListElement
{
public:
	PAR_ID_BYTES current;
	CANListElement* next;
	//bool single;	// ���� ��� ���������, ����� �� ������ �������

	static CANListElement* first;	// ������ ������ ������ �� ������ �������, ����� ����� ���� ������ �����, ���� ���

public:
	CANListElement(PAR_ID_BYTES id, CANListElement* parent = 0) //,isSingle= false
	{
		current = id;
		next = 0;
		//single = isSingle;

		if (!parent)
		{
			parent = Last();
		}

		if (!parent)
		{
			first = this;
		}
		else
		{
			parent->next = this;
		}
	}

	static CANListElement* Last()
	{
		CANListElement* last = first;
		if (!last) return 0;

		while (last->next != 0)
		{
			last = last->next;
		}
		return last;
	}

	static void Clear()
	{
		while (first != 0)
		{
			//CANListElement* nxt = first->next;
			//delete first;
			//first = nxt;
			// ���������, ����� ������� �����?
			// �� ������ �������� first ������ ������������� ����-�� �����������,
			// � ������ �� ��������� ����� ����������.
			// �� ������ ������ ������� ��������� �� �����?..
			CANListElement* cur = first;
			first = first->next;
			delete cur;
		}
	}
};

/**
 * ������ �� ��������� ���������. ������ ������ � ����� ������:
 * (0xFF) - (P) - (S) - (SS) - (SSS)
 */
#define EC_PREQ		0xFB		// ��������� ������ ���������
#define EC_PQUE		0xFA		// ���������� ��������� � ������ ���������� ��������
#define EC_PCLR		0xF9		// ��������� ������ ���������� ��������


#define EC_TIME		0xFE		// ����� ������� �� ������ ������� ���������� �� ����
#define EC_TINJ		0xFD		// ����� ������� �� ������ �������
#define EC_TPROG	0xFC		// ����� ������� �� ����� ����� ���������
#define EC_THALL	0xF8		// ����� ������� �� "�������" ����

#define EC_P0		0x00

#define EC_BAD		0xFF
/**
 * ����������� ����������:
 * - EC = Electronic Control
 * - P - Parameter - ��������
 * - G - ������ ����������
 * - T - Table - �������. ����� ������������ ��� �������������
 * ���������� ��� ������� �� ���������
 * - S - Sub-parameter / Sub-index 1
 * ���������� ������������� ���������� � 1,
 * ����������� � �������� � ������� � 0
 * - SS - Sub-index 2
 */

#define EC_P_MODE		0x01			// �����
#define EC_G_N			0x02			// ������ ����������: ������� ��������
#define 	EC_S_NR				0x00	// ������� �������
#define 	EC_S_NU				0x01	// �������
#define 	EC_S_OMEGA			0x02	// ������� � ���/�
#define 	EC_S_DTIME			0x03	// ����� ����� ������� (������ �����)
#define		EC_S_NMIN			0x04	// ����������� �������� �������, ���� �������� ������� �������� �����������
#define 	EC_S_NMAX			0x05	// ������� ����������� ������� ��������
#define 	EC_S_NRESTR			0x06	// ����������� ������� ��������

#define EC_G_QC			0x03			// ������ ����������: ������
#define 	EC_S_QC_T			0x00	// ����������������� ������������� ��������
#define 	EC_S_QC_AN			0x01	// ����������������� � �����
#define 	EC_S_QC				0x02	// ������ ��3/����
#define		EC_S_ADOP			0x03	// ����������� �� �����
#define		EC_S_QC_ADOP		0x04	// ������������ ��� �������� QCmax // ����������� ������ �� �����
#define 	EC_S_QC_MAX			0x05	// ������������ �������� ������
#define 	EC_S_QC_MIN			0x06	// ����������� �������� ������
#define		EC_S_KQC			0x07	// ���������� ����������� ��/� -> ���
#define 	EC_S_START			0x08	// ��������� �������� ������ � ��3/����
#define EC_P_NCYL		0x04			// ���������� ���������
	// ��������� (i-1) ����� �������������� ��� �������� ������, ���������������� ������� ��������
#define EC_P_PED		0x05			// �������� ������
#define 	EC_S_PEDST			0x01	// ��� ��������� �������� ������
#define EC_P_VMT		0x06			// ���� �������� ��� ������������ ����������� ������

#define EC_G_INJ		0x10			// ��������� �������
#define 	EC_S_INJT1			0x00	// ����������������� ������������
#define 	EC_S_INJT2			0x01	// ����������������� �������������
#define 	EC_S_INJD1			0x02	// ���������� ������������
#define 	EC_S_INJD2			0x03	// ���������� �������������

// ��������� - ����� �������� 0..NCYL
#define EC_T_INJPHI		0x11			// ���� �������� - ������ �������
#define EC_T_INJZ		0x12			// ����� ����, �� �������� ������������� ������
#define EC_T_INJN		0x13			// ���������� �������� � ��������������� �������
#define EC_T_INJT		0x14			// �������� � ��� �� ���������� � InjZ ����
#define EC_T_INJCNT		0x15			// ���������� �������� ������� � ������� ��������� ����� �� ������


#define EC_P_M_MODE		0x21			// ������ ����� ������ �����
#define EC_P_M_QC		0x22			// ������ ��������� ������
#define 	EC_S_M_AN			0x01	// ������ ��������� ������ � �����
#define 	EC_S_M_IAN			0x02	// ���� ��� ��������� � �������� ������
#define		EC_S_M_QCT			0x03	// ������ ��������� ������ � ���
#define 	EC_S_M_QALPHA		0x04	// ���/���� ���������������
#define EC_P_M_INJ		0x23			// ������ ����� �������
#define 	EC_S_M_IONCE		0x01	// ��������� ������ ��� ������
#define 	EC_S_M_IN			0x02	// ������� ��������
#define 	EC_S_M_INJCYL		0x03	// �������, � ������� �������������� ������
#define 	EC_S_FDBKCYL		0x04	// �������, � �������� ����� ����������� �������� �����
#define EC_P_M_PED		0x24			// ������ ��������� �������� ������
#define EC_P_M_CANS		0x25			// �������� ��������� CAN
#define EC_S_M_CANST			0x01	// �������� ����� �������
#define EC_P_M_UOVT		0x26			// ������ ����� ��������� ����
#define 	EC_S_M_UOVT			0x01	// ���� ���������� ������� �������
#define EC_P_M_FDBK		0x27			// ����� ������ �������� �����
	// ��������� ��������� ����� ����� � ������� �������� �����
#define EC_P_M_LED		0x28			// ��������� �����������
#define EC_P_M_SENS		0x29			// ������ �� ��������� ��������
#define 	EC_S_D_PEDAL		0x01	// ������
#define 	EC_S_D_KEY			0x02	// ����
#define 	EC_S_D_PK			0x03	// �������� �������
#define 	EC_S_D_PINJ			0x04	// �������� ����� / �������� ���������� ���� �� ��������
#define 	EC_S_D_TV			0x05	// ����������� �������
#define 	EC_S_D_PINJMAX		0x06	// ������� ����������� �������� �����
#define 	EC_S_D_PINJMIN		0x07	// ������ ����������� �������� �����
#define 	EC_S_D_PINJRESTR	0x08	// ���/���� ����������� �� �������� �����

#define EC_T_UOVT		0x31			// ������� ����
#define EC_T_SPCHAR		0x32			// ������� - ���������� ��������������
#define EC_T_NSTEADY	0x33			// ������� - ����������� ������� �� �� ?
#define EC_T_QSTART		0x34			// ������� - ����������� ��������� ������ �� ?

// ���
#define EC_P_KP			0x41			// ��
#define EC_P_KI			0x42			// ��
#define EC_P_KD			0x43			// ��
#define EC_P_ERR		0x44			// ���� * ��
#define EC_P_ERRI		0x45			// ���� * ��
#define 	EC_S_ERRIMAX	0x01		// ����������� ������������ ������������ default = QCmax*1.1
#define EC_P_ERRD		0x46			// ���� * ��

#define EC_P_MUN		0x47			// ����������� ������ ������� ��������
#define EC_P_DZONE		0x48			// ���� ������������������

#define EC_P_PTIME		0x51			// ����� ������ ���������
#define EC_P_ITIME		0x52			// ����� XINT1
#define EC_P_ITIME1		0x53			// ����� TIM1

// ������ ����������, ���������� �� ������ ������������ �������
#define EC_G_CHAN		0x61
#define 	EC_T_VAL			0x00	// ��������
#define 	EC_T_CHAN			0x01	// �����
#define 	EC_T_SENS			0x02	// ��� �������
#define 	EC_S_INOUT			0x03	// 0 - �������, 1 - �������������� ����������, default - �� ������


#endif /* CAN_PAR_ID_H_ */
