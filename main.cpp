/**
 * ������� ������� ���������� ��������� ����������
 *
 * main.cpp
 */

#include "main.h"

EC_Engine* engine;

int main(void) {

	// �������������
	engine = new EC_Engine();

	// ������ ��������� �����
	engine->Run();
}
