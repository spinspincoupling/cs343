#pragma once
#include "global.h"
#include "train.h"

_Task Student {
    Printer &prt;
    NameServer &nameServer;
    WATCardOffice &cardOffice;
    Groupoff &groupoff;
    unsigned int id;
    unsigned int numStops;
    unsigned int stopCost;
    unsigned int maxStudentDelay;
    unsigned int maxStudentTrips;
    void main();
  public:
    Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
        unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips );
    ~Student();
};

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
