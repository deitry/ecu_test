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
		engine->ControlCheck();
		engine->Monitoring();

		switch (engine->mode)
		{
		case EC_Start:	// ����
			{
				engine->Start();
				break;
			}
		case EC_Steady:	// ������������ ����� (�������� ���)
			{
				engine->Steady();
				break;
			}
		case EC_Automotive:	// �������������� (�������) ����� - ������-������
		case EC_Transient:	// �������������� (�������) ����� - ������-�������
			{
				engine->Transient();
				break;
			}
		case EC_Failure:	// ��������� ����� (��������� ���������� ������?)
			{
				engine->Failure();
				break;
			}
		case EC_Finish:	// ���������� ������ (����������)
			{
				engine->Finish();
				break;
			}
		case EC_End:
			{
				// ���������
				for (;;)
				{
					if (engine->mode != EC_End)
						break;
				}
			}
		}
	}
}


