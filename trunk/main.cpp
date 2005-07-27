#include "GameController.h"
#include "Sand.h"
#include "Cover.h"
#include "SquishMaze.h"

#include <getopt.h>

#include <iostream>
using namespace std;

static const char short_options [] = ":scmh";

static const struct option long_options [] = {
	{"help",  no_argument, NULL, 'h' },
	{"sand",  no_argument, NULL, 's' },
	{"cover", no_argument, NULL, 'c' },
	{"maze",  no_argument, NULL, 'm' },
	{0, 0, 0, 0}
};

int main(int argc, char **argv)
{
	cout << "Shadowplay v2 - Electric Boogaloo\n";
	GameController *gamecontroller;

	bool started = false;

	for (;;)
	{
		int index;
		int c = getopt_long (argc, argv,
				short_options, long_options,
				&index);

		if (started == true || c < 0)
			break;

		switch (c)
		{
			case 0:
				break;

			case 's':
				cout << "main: Creating the Sand object\n";
				gamecontroller = new Sand();
				started = true;
				break;

			case 'c':
				cout << "main: Creating the Cover object\n";
				gamecontroller = new Cover();
				started = true;
				break;

			case 'm':
				cout << "main: Creating the SquishMaze object\n";
				gamecontroller = new SquishMaze();
				started = true;
				break;

			case 'h':
				cout << "-s | --sand	Play with sand as it falls.\n";
				cout << "-c | --cover	Two player game where you have to cover some circles.\n";
				cout << "-m | --maze	A two player squishy maze. Get to the other side first!\n";
				exit(EXIT_SUCCESS);

			default:
				break;
		}
	}

	if (started == false)
	{
		cout << "main: Creating the GameController object\n";
		gamecontroller = new GameController();
	}


	cout << "main: Starting to play\n";
	gamecontroller->StartPlaying();

	cout << "main: Exiting normally\n";
	return 0;
}
