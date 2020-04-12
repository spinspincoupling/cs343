#pragma once
#include "global.h"

_Task Train;

_Task Conductor {
	friend _Task Train;
	Printer & prt;
	unsigned int id;
	Train * train;
	unsigned int delay;
	bool active;

	void main();
  public:
  	Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay );
  	~Conductor();
};


// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
