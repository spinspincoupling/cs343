#pragma once

_Monitor Printer {
  public:
	enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Timer, Train, Conductor, TrainStop, Student, WATCardOfficeCourier };

  private:
  	const unsigned int numStudents, numTrains, numStops, numCouriers, d;
	const unsigned int offset1, offset2, offset3, offset4, offset5, total;
    unsigned int cnt;					// number of filled buffer elements
    bool *written;

    // Defines information that needs to be tracked.
    struct Item { //minimize size
	  unsigned int type;
	  char state;
	  unsigned int v1,v2;
	  union {
	    unsigned int v3;
	    char c;
	  };
    } curr, *columns;

	void printItem(const Item& item);
	void flushBuffer();
	unsigned int getOffset(Kind kind);
	void insert(unsigned int index);

  public:
	Printer( unsigned int numStudents, unsigned int numTrains, unsigned int numStops, unsigned int numCouriers );
	~Printer();
	void print( Kind kind, char state );
	void print( Kind kind, char state, unsigned int value1 );
	void print( Kind kind, char state, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1 );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state, char c );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, char c );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c );
}; // Printer

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
