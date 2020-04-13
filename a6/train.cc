#include "train.h"
#include "trainstop.h"
#include "printer.h"
#include "nameserver.h"
#include "conductor.h"

extern unsigned int conductorDelay;

Train::Train( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops )
    :prt{prt}, nameServer{nameServer}, id{id}, maxNumStudents{maxNumStudents}, numStops{numStops}, numStudents{0}, 
    active{true}, clockwise{id==0}, stopId{clockwise? 0:(numStops+1)/2} {
        conductor = new Conductor(prt, id, this, conductorDelay);
        prt.print(Printer::Kind::Train, id, 'S', stopId, clockwise? '<':'>');
        stops = new uCondition[numStops];
        counts = new int[numStops] {0};
}
	
Train::~Train(){
    delete[] stops;
    delete[] counts;
    prt.print(Printer::Kind::Train, id, 'F');
}

unsigned int Train::getId() const {
    return id;
}

TrainStop* Train::embark( unsigned int studentId, unsigned int destStop, WATCard& card ){
    prt.print(Printer::Kind::Train, id, 'E', studentId, destStop);
    ++numStudents;
    ++counts[destStop]; //increase counter waiting for destStop
    for(;;){
        stops[destStop].wait();
        if(current->getId() == destStop) break; //wake up because of arrival
        if(!card.paidForTicket()){ //wake up because of conductor
            --numStudents; //removed from people in train
            --counts[destStop]; 
            prt.print(Printer::Kind::Conductor, id, 'e', studentId);
            uRendezvousAcceptor(); //disable Rendezvous failure
            throw Ejected();
            break;
        }
    }
    --numStudents;
    --counts[destStop]; //get off the train
    card.resetPOP();
    return current;
}

void Train::scanPassengers(){
    if(active){
        for(unsigned int i=0; i< numStops; ++i){
            unsigned int p = counts[i];
            for(int j=0; j<p; ++j){
                stops[i].signalBlock(); //check ticket
            }
        }
    } else {
        uRendezvousAcceptor();
        throw Ejected(); //indicate termination
    } 
}

void Train::main(){
    TrainStop** trainStops = nameServer.getStopList(id);
    unsigned int adder, canTake;
    Direction dir;
    if(clockwise){
        adder = 1;
        dir = Direction::Clockwise;
    } else {
        adder = numStops-1;
        dir = Direction::CounterClockwise;
    }
    for(;;){
        _Accept(~Train){
            active = false;
            _Accept(scanPassengers){ //let it throw exception
                delete conductor;
            }
            break;
        }
        or _Accept(embark, scanPassengers){ 
        }
        _Else{ //arrive
            current = trainStops[stopId];
            canTake = maxNumStudents-numStudents;
            prt.print(Printer::Kind::Train, id, 'A', stopId, canTake, numStudents);
            current->arrive(id, dir, canTake);
            while(!stops[stopId].empty()){ //wake student to disembark
                stops[stopId].signalBlock();
            }
            stopId = (stopId+adder)%numStops;
        } 
    }
}