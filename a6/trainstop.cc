#include "trainstop.h"
#include "train.h"
#include "printer.h"
#include "nameserver.h"
#include "watcard.h"

TrainStop::TrainStop( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost )
    :prt{prt}, nameServer{nameServer}, id{id}, stopCost{stopCost}, wait0{0}, wait1{0} {}

TrainStop::~TrainStop(){
    prt.print(Printer::Kind::TrainStop, id, 'F');
}

unsigned int TrainStop::getId() const {
    return id;
}

void TrainStop::buy( unsigned int numStops, WATCard & card ){
    unsigned int cost = stopCost*numStops;
    unsigned int balance = card.getBalance();
    if(balance < cost){
        throw Funds(cost - balance);
    } else {
        card.withdraw(cost);
        prt.print(Printer::Kind::TrainStop, id, 'B', cost);
    }
}

Train* TrainStop::wait( unsigned int studentId, Train::Direction direction ){
    if(direction == Train::Direction::Clockwise){
        prt.print(Printer::Kind::TrainStop, id, 'W', studentId, '<');
        ++wait0;
        clockwise.wait();
    } else {
        prt.print(Printer::Kind::TrainStop, id, 'W', studentId, '>');
        ++wait1;
        anticlockwise.wait();
    }
    return arrived;
}

void TrainStop::disembark( unsigned int studentId ){
    prt.print(Printer::Kind::TrainStop, id, 'D', studentId);
}

void TrainStop::tick(){
    prt.print(Printer::Kind::TrainStop, id, 't');
}

unsigned int TrainStop::arrive( unsigned int trainId, Train::Direction direction, unsigned int maxNumStudents ){
    unsigned int signalled;
    if(direction == Train::Direction::Clockwise){
        prt.print(Printer::Kind::TrainStop, id, 'A', trainId, maxNumStudents, wait0);
        signalled = maxNumStudents > wait0 ? wait0:maxNumStudents;
        for(unsigned int i=0; i<signalled; ++i){
            clockwise.signal();
        }
    } else {
        prt.print(Printer::Kind::TrainStop, id, 'A', trainId, maxNumStudents, wait1);
        signalled = maxNumStudents > wait1 ? wait1:maxNumStudents;
        for(unsigned int i=0; i<signalled; ++i){
            anticlockwise.signal();
        }
    }
    arrived = (Train*) &(getCoroutine()); //should be caller instance
    train.wait();
    return signalled;
}

void TrainStop::main(){
    prt.print(Printer::Kind::TrainStop, id, 'S');
    nameServer.registerStop(id);
    for(;;) {
        _Accept(~TrainStop){
            break;
        }
        or _Accept(arrive){ 
            _Accept(tick){ //wait for tick
                train.signalBlock();
            }
        }
        or _Accept(wait){
        }
        or _Accept(buy){
        }
        or _Accept(tick){
        }
    }
}