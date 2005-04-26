#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>

#include "VideoInput.h"
#include "Controller.h"

#include <iostream>
using namespace std;


Controller::Controller(void)
{
	cout << "Creating the Shadow object\n";
	shadow = new Shadow();
	cout << "Creating the GameController object\n";
	gamecontroller = new GameController();
}

Controller::~Controller(void)
{
}

