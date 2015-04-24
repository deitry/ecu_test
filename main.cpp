/**
 * ������� ������� ���������� ��������� ����������
 *
 * main.cpp
 */

#include "main.h"
//#include <memcopy.h>

//using namespace EG;
EC_Engine* engine;

//#pragma CODE_SECTION("ramfuncs")
int main(void) {
	
	// ������������� ����������
	//MemCopy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
	InitSysCtrl();

	// �������������
	engine = new EC_Engine();
	engine->mode = EC_Transient;

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
				EG::errI = 0;	// ���������� ������������ ������������
				engine->Steady();
				break;
			}
		case EC_Automotive:	// �������������� (�������) ����� - ������-������
		case EC_Transient:	// �������������� (�������) ����� - ������-�������
			{
				EG::errI = 0;	// ���������� ������������ ������������
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
				// ���������� ������������ �� ���� �����
				//EG::kP = 1e-5;
				//EG::kI = 1e-6;
				//EG::kD = 0;
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
