#include "trainstop.h"
#include "train.h"
#include "printer.h"
#include "nameserver.h"
#include "watcard.h"
#include <iostream>

TrainStop::TrainStop( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost )
    :prt{prt}, nameServer{nameServer}, id{id}, stopCost{stopCost}, wait0{0}, wait1{0} {
        prt.print(Printer::Kind::TrainStop, id, 'S');
        nameServer.registerStop(id);
    }

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
        //std::cout << "not enough fund!!" << '\n';
        throw Funds(cost - balance);
    } else {
        card.withdraw(cost);
        prt.print(Printer::Kind::TrainStop, id, 'B', cost);
    }
    //uRendezvousAcceptor();
}

Train* TrainStop::wait( unsigned int studentId, Train::Direction direction ){
    if(direction == Train::Direction::Clockwise){
        prt.print(Printer::Kind::TrainStop, id, 'W', studentId, '<');
        if(!train0.empty() && signalled0 < limit0) {
            ++signalled0;  
        } else {
            ++wait0;
            clockwise.wait();
            --wait0;
        }
        return arrived0;
    } else {
        prt.print(Printer::Kind::TrainStop, id, 'W', studentId, '>');
        if(!train1.empty() && signalled1 < limit1) {
            ++signalled1;
        }else {
            ++wait1;
            anticlockwise.wait();
            --wait1;
        }
        return arrived1; 
    }
}

void TrainStop::disembark( unsigned int studentId ){
    prt.print(Printer::Kind::TrainStop, id, 'D', studentId);
}

void TrainStop::tick(){
    prt.print(Printer::Kind::TrainStop, id, 't');
}

unsigned int TrainStop::arrive( unsigned int trainId, Train::Direction direction, unsigned int maxNumStudents ){
    if(direction == Train::Direction::Clockwise){
        limit0 = maxNumStudents;
        prt.print(Printer::Kind::TrainStop, id, 'A', trainId, maxNumStudents, wait0);
        signalled0 = maxNumStudents > wait0 ? wait0:maxNumStudents;
        for(unsigned int i=0; i<signalled0; ++i){
            clockwise.signal();
        }
        arrived0 = &(static_cast<Train>(uThisTask())); //should be caller instance
        train0.wait();
        return signalled0;
    } else {
        limit1 = maxNumStudents;
        prt.print(Printer::Kind::TrainStop, id, 'A', trainId, maxNumStudents, wait1);
        signalled1 = maxNumStudents > wait1 ? wait1:maxNumStudents;
        for(unsigned int i=0; i<signalled1; ++i){
            anticlockwise.signal();
        }
        arrived1 = &(static_cast<Train>(uThisTask())); //(Train*) &(uThisTask()); //should be caller instance
        train1.wait();
        return signalled1;
    }
}

void TrainStop::main(){
    for(;;) {
        try{
            _Accept(~TrainStop){
                break;
            }
            or _Accept(buy){
            }
            or _Accept(wait){
            }
            or _Accept(disembark){
            }
            or _Accept(arrive){
            }
            or _Accept(tick){
                if(!train0.empty()) train0.signalBlock();
                if(!train1.empty()) train1.signalBlock();
            }
        } catch (uMutexFailure::RendezvousFailure &){
        }
        
    }
}