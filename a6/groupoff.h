#pragma once

#include "watcard.h"
#include "printer.h"

_Task Groupoff {
	Printer & prt;
	unsigned int numStudents;
	unsigned int maxTripCost;
	unsigned int groupoffDelay;
	int counter; // number of giftcard needs to be deliver
	WATCard::FWATCard *futures;
	WATCard** cards; //actual watcards
	unsigned int *remaining; //indexes of unfilled futures

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
