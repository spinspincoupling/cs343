#include "conductor.h"
#include "train.h"
#include "printer.h"

Conductor::Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay )
    :prt{prt}, id{id}, train{train}, delay{delay} {
        prt.print(Printer::Kind::Conductor, id, 'S');
    }

Conductor::~Conductor(){
    prt.print(Printer::Kind::Conductor, id, 'F');
}

void Conductor::main(){
    try{
        for(;;){
            yield(delay);
            prt.print(Printer::Kind::Conductor, id, 'c');
            train->scanPassengers();
        } 
    } catch (Train::Ejected &){ //communicate termination
    }   
}