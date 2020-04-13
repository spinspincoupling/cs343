#include "time.h"
#include "printer.h"
#include "nameserver.h"
#include "trainstop.h"

Timer::Timer( Printer & prt, NameServer & nameServer, unsigned int timerDelay )
    :prt{prt}, nameSever{nameServer}, timerDelay{timerDelay} {
    prt.print(Printer::Kind::Timer, 'S');
}

Timer::~Timer(){
    prt.print(Printer::Kind::Timer, 'F');
}

void Timer::main(){
    unsigned int counter = 0, numStops = getNumStops();
    TrainStop* stops[numStops] = nameServer.getStopList();
    for(;;){
        _Accept(~Timer){
            break;
        }
        _Else {
            yield(timerDelay);
            prt.print(Printer::Kind::Timer, 't', counter);
            for(unsigned int i=0; i<numStops; ++i){
                stops[i]->tick();
            }
            ++counter;
        }
    }
}