/**
 * Ïðîáíûé âàðèàíò ðåàëèçàöèè ïðîãðàììû óïðàâëåíèÿ
 *
 * main.cpp
 */

#include "main.h"

EC_Engine* engine;

//#pragma CODE_SECTION("ramfuncs")
int main(void) {

	// ÈÍÈÖÈÀËÈÇÀÖÈß
	engine = new EC_Engine();

	// çàïóñê ÎÑÍÎÂÍÎÃÎ öèêëà
	engine->Run();
}
