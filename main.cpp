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

	engine->Run();
}
