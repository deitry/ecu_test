/**
 * Ïðîáíûé âàðèàíò ðåàëèçàöèè ïðîãðàììû óïðàâëåíèÿ
 *
 * main.cpp
 */

#include "main.h"

EC_Engine* engine;

int main(void) {

	// ÈÍÈÖÈÀËÈÇÀÖÈß
	engine = new EC_Engine();

	// çàïóñê ÎÑÍÎÂÍÎÃÎ öèêëà
	engine->Run();
}
