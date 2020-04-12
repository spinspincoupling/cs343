#include "conductor.h"
#include "train.h"
#include "printer.h"
#include <iostream>

Conductor::Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay )
    :prt{prt}, id{id}, train{train}, delay{delay}{}

Conductor::~Conductor(){
    prt.print(Printer::Kind::Conductor, id, 'F');
    std::cout << "end conductor destructor" << '\n' ;
}

void Conductor::main(){
    prt.print(Printer::Kind::Conductor, id, 'S');
    for(;;){
        _Accept(~Conductor){
            std::cout << "breaking out of conductor destructor" << '\n';
            break;
        }
        _Else {
            yield(delay);
            prt.print(Printer::Kind::Conductor, id, 'c');
            train->scanPassengers();
        }
    }
}