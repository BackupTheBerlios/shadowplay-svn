#include "GameController.h"
#include "Sand.h"

#include <iostream>
using namespace std;

int main()
{
	cout << "main: Starting Shadowplay\n";

	cout << "main: Creating the GameController object\n";
	//GameController *gamecontroller = new GameController();
	GameController *gamecontroller = new Sand();
	cout << "main: Starting to play\n";
	gamecontroller->StartPlaying();

	cout << "main: Exiting normally\n";
	return 0;
}
