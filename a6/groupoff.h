#pragma once

#include "watcard.h"
#include "printer.h"
#include <list>

_Task Groupoff {
	Printer & prt;
	unsigned int numStudents;
	unsigned int maxTripCost;
	unsigned int groupoffDelay;
	int counter;
	WATCard::FWATCard *futures;

	void main();
  public:
	Groupoff( Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay );
	~Groupoff();
	WATCard::FWATCard giftCard();
}; // Groupoff

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
