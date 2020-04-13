#include "nameserver.h"
#include "printer.h"
#include "trainstop.h"

NameServer::NameServer( Printer & prt, unsigned int numStops, unsigned int numStudents )
    :prt{prt}, numStops{numStops}, numStudents{numStudents}{
        stopLits = new TrainStop*[numStops];
}

NameServer::~NameServer(){
    delete[] stopLits;
    prt.print(Printer::Kind::NameServer, 'F');
}

void NameServer::registerStop( unsigned int trainStopId ){
    prt.print(Printer::Kind::NameServer, 'R', trainStopId);
    stopLits[trainStopId] = static_cast<TrainStop*>(&uThisTask());
}

TrainStop* NameServer::getStop( unsigned int studentId, unsigned int trainStopId ){
    prt.print(Printer::Kind::NameServer, 'T', studentId, trainStopId);
    return stopLits[trainStopId];
}

TrainStop** NameServer::getStopList(){
    prt.print(Printer::Kind::NameServer, 'L');
    return stopLits;
}

TrainStop** NameServer::getStopList( unsigned int trainId ){
    prt.print(Printer::Kind::NameServer, 'L', trainId);
    return stopLits;
}

unsigned int NameServer::getNumStops(){
    return numStops;
}

void NameServer::main(){
    prt.print(Printer::Kind::NameServer, 'S');
    for(unsigned int i=0; i<numStops; ++i){ //wait for all stops to be registered before processing requests
        _Accept(registerStop);
    }
    for(;;){
        _Accept(~NameServer){
            break;
        }
        or _Accept(getStop, getStopList, getNumStops);
    }
}
