#include "train.h"
#include "trainstop.h"
#include "MPRNG.h"
#include "printer.h"
#include "nameserver.h"
#include "conductor.h"
#include "global.h"
#include <iostream>

Train::Train( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops )
    :prt{prt}, nameServer{nameServer}, id{id}, maxNumStudents{maxNumStudents}, numStops{numStops}, numStudents{0} {
        stops = new uCondition[numStops];
        counts = new int[numStops] {0};
        conductor = new Conductor(prt, id, this, conductorDelay);
    }
	
Train::~Train(){
    delete[] stops;
    delete[] counts;
    delete conductor;
    std::cout << "destructor after delete conductor" << '\n';
    prt.print(Printer::Kind::Train, id, 'F');
}

unsigned int Train::getId() const {
    return id;
}

TrainStop* Train::embark( unsigned int studentId, unsigned int destStop, WATCard& card ){
    prt.print(Printer::Kind::Train, id, 'E', studentId, destStop);
    ++numStudents;
    ++counts[destStop];
    for(;;){
        stops[destStop].wait();
        if(current->getId() == destStop) break;
        if(!card.paidForTicket()){
            prt.print(Printer::Kind::Conductor, id, 'e', studentId);
            throw Ejected();
            break;
        }
    }
    --numStudents;
    --counts[destStop];
    return current;
}

void Train::scanPassengers(){
    if(numStudents > 0){
        for(unsigned int i=0; i< numStops; ++i){
            for(int j=0; j<counts[i]; ++j){
                stops[i].signalBlock();
            }
        }
    }
    
}

void Train::main(){
    const bool clockwise = (id==0);
    unsigned int stopId = clockwise? 0:(numStops+1)/2;
    prt.print(Printer::Kind::Train, id, 'S', stopId, clockwise? '<':'>');
    TrainStop** trainStops = nameServer.getStopList(id);
    unsigned int adder, canTake;
    Direction dir;
    if(clockwise){
        current = trainStops[0];
        adder = 1;
        dir = Direction::Clockwise;
    } else {
        current = trainStops[stopId];
        adder = numStops-1;
        dir = Direction::CounterClockwise;
    }
    
    for(;;){
        //Conductor conductor(prt, id, this, conductorDelay);
        try{
            _Accept(~Train){
                conductor.active = false;
                break;
            }
            or _Accept(embark){//stravation??
            }
            or _Accept(scanPassengers){
            }
            _Else{
                canTake = maxNumStudents-numStudents;
                prt.print(Printer::Kind::Train, id, 'A', stopId, canTake, numStudents);
                current->arrive(id, dir, canTake);
                while(!stops[stopId].empty()){ //wake student to disembark
                    stops[stopId].signalBlock();
                }
                stopId = (stopId+adder)%numStops;
                current = trainStops[stopId];
            }
        }catch (uMutexFailure::RendezvousFailure &){
        }
        
    }
}