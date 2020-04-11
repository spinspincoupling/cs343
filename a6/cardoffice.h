#pragma once

#include "watcard.h"
#include <list>

_Monitor Printer;										// forward declarations
_Monitor Bank;

_Task WATCardOffice {
	struct Job {
		unsigned int sid;
		unsigned int amount;
		WATCard *card;
		WATCard::FWATCard result; // return future
		Job(unsigned int sid, unsigned int amount, WATCard *card = nullptr) : sid{sid}, amount{amount}, card{card}
		{}
	};
	_Task Courier {
		Printer &prt;
		Bank &bank;
		WATCardOffice *office;
		unsigned int id;
		void main();
		public:
		Courier(Printer &prt, Bank &bank, WATCardOffice *office, unsigned int id);
		~Courier();
		void stop() {}
	};
	Printer &prt;
	Bank &bank;
	unsigned int numCouriers;
	std::list<Job*> workQueue;
	uCondition waiting;
	void main();
  public:
	_Event Lost {};										// lost WATCard
	WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers );
	~WATCardOffice();
	WATCard::FWATCard create( unsigned int sid, unsigned int amount ); // called by student
	WATCard::FWATCard transfer( unsigned int sid, unsigned int amount, WATCard * card ); // called by student
	Job * requestWork();								// called by courier to request/return work
}; // WATCardOffice

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
