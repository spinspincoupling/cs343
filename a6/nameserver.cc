#include "nameserver.h"
#include "printer.h"

NameServer::NameServer( Printer & prt, unsigned int numStops, unsigned int numStudents )
    :prt{prt}, numStops{numStops}, numStudents{numStudents}{
        stopLits = new TrainStop*[numStops];
    }

NameServer::~NameServer(){
    delete[] stopLits;
    prt.print(Printer::Kind::NameServer, 'F');
}

void NameServer::registerStop( unsigned int trainStopId ){
    prt.print(Printer::Kind::NameServer, 'R', transStopId);
    stopLits[trainStopId] = static_cast<TrainStop*>(&uThisTask());
}

TrainStop* NameServer::getStop( unsigned int studentId, unsigned int trainStopId ){
    prt.print(Printer::Kind::NameServer, 'T', studentId, trainStopId);
    return stopLits[transStopId];
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

void main(){
    prt.print(Printer::Kind::NameServer, 'S');
    for(unsigned int i=0; i<numStops; ++i){
        _Accept(registerStop);
    }
    for(;;){
        _Accept(~NameServer){
            break;
        }
        or _Accept(getStop, getStopList, getNumStops);
    }
}
