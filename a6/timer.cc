#include "timer.h"
#include "printer.h"
#include "nameserver.h"
#include "trainstop.h"

Timer::Timer( Printer & prt, NameServer & nameServer, unsigned int timerDelay )
    :prt{prt}, nameServer{nameServer}, timerDelay{timerDelay} {
    prt.print(Printer::Kind::Timer, 'S');
}

Timer::~Timer(){
    prt.print(Printer::Kind::Timer, 'F');
}

void Timer::main(){
    unsigned int counter = 0, numStops = nameServer.getNumStops();
    TrainStop** stops = nameServer.getStopList();
    for(;;){
        _Accept(~Timer){
            break;
        }
        _Else {
            yield(timerDelay);
            prt.print(Printer::Kind::Timer, 't', counter);
            for(unsigned int i=0; i<numStops; ++i){ //tick all stops
                stops[i]->tick();
            }
            ++counter;
        }
    }
}