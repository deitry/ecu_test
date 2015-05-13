/*
 * run.cpp
 *
 *  Created on: 11 ��� 2015 �.
 *      Author: �������� ������
 */

#include "../main.h"

/**
 * �������� ���� ��������� - ������������ ��������� �����.
 */
#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Run()
{
	// �������� ����
	for (;;)
	{
		this->ControlCheck();
		this->Monitoring();

		switch (this->mode)
		{
		case EC_Start:	// ����
			{
				this->Start();
				break;
			}
		case EC_Steady:	// ������������ ����� (�������� ���)
			{
				this->Steady();
				break;
			}
		case EC_MaxDyn:
		case EC_Automotive:	// �������������� (�������) ����� - ������-������
		case EC_Transient:	// �������������� (�������) ����� - ������-�������
			{
				this->Transient();
				break;
			}
		case EC_Failure:	// ��������� ����� (��������� ���������� ������?)
			{
				this->Failure();
				break;
			}
		case EC_Finish:	// ���������� ������ (����������)
			{
				this->Finish();
				break;
			}
		case EC_End:
			{
				// ���������
				for (;;)
				{
					if (this->mode != EC_End)
						break;
				}
			}
		}
	}
}


