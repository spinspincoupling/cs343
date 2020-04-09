#pragma once

// Forward declarations
class WATCard;

_Task Timer;
_Task TimerCourier;
_Task NameServer;
_Task TrainStop;
_Task Train;
_Task Conductor;
_Task Student;
_Task WATCardOffice;
_Task Groupoff;
_Task Parent;

_Monitor Printer;
_Monitor Bank;
_Monitor MPRNG;
extern MPRNG mprng;					// random-number generator
const unsigned int numTrains = 2;
unsigned int maxTripCost;


// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
