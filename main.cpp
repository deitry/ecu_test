/**
 * ������� ������� ���������� ��������� ����������
 *
 * main.cpp
 */

#include "main.h"

EC_Engine* engine;

//#pragma CODE_SECTION("ramfuncs")
int main(void) {

	// �������������
	engine = new EC_Engine();

	// ������ ��������� �����
	engine->Run();
}
