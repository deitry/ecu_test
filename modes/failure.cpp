/*
 * failure.cpp
 *
 *  Created on: 12.09.2014
 *      Author: ����
 */

#include "../main.h"
#include "math.h"

using namespace EG;

/**
 * ���� ��� ����� ���������, ������ ����������� �� ������ � 2 ���� ������
 */
//#pragma CODE_SECTION("ramfuncs")
int EC_Engine::Failure()
{
	for(;;)
	{
		// �������� ��������� ������� ����������
		if (!this->ControlCheck())
		{
			// ��������� ������ ������ - ���������� ������� � �.�.
			//this->ModeCalc();
			if (mode != EC_Failure)
				return 0;
		}

		// - ��������� ����� �������
		setInjPhi();
		// - ��������� �������� ������
		this->ModeCalc();
		this->Monitoring();
	}
}
