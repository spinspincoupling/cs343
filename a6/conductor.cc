#include "conductor.h"
#include "train.h"
#include "printer.h"
#include <iostream>

Conductor::Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay )
    :prt{prt}, id{id}, train{train}, delay{delay}, active{active} {}

Conductor::~Conductor(){
    prt.print(Printer::Kind::Conductor, id, 'F');
}

void Conductor::main(){
    prt.print(Printer::Kind::Conductor, id, 'S');
    /*for(;;){
        _Accept(~Conductor){
            break;
        }
        _Else{
            yield(delay);
            prt.print(Printer::Kind::Conductor, id, 'c');
            train->scanPassengers();
        }
    }*/
    while(active){
        yield(delay);
        prt.print(Printer::Kind::Conductor, id, 'c');
        train->scanPassengers();
    } 
}