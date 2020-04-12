#include "train.h"
#include "trainstop.h"
#include "MPRNG.h"
#include "printer.h"
#include "nameserver.h"

Train::Train( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops )
    :prt{prt}, nameServer{nameServer}, id{id}, maxNumStudents{maxNumStudents}, numStops{numStops}, numStudents{0} {
        stops = new uCondition[numStops];
        counts = new int[numStops];
    }
	
Train::~Train(){
    delete stops;
    delete counts;
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
            throw Ejected();
        }
    }
    --numStudents;
    --counts[destStop];
}

void Train::scanPassengers(){
    for(unsigned int i=0; i< numStops; ++i){
        for(int j=0; j<counts[i]; ++j){
            stops[j].signal();
        }
    }
}

void Train::main(){
    prt.print(Printer::Kind::Train, id, 'S');
    TrainStop* trainStops[] = getStopList(id);
    unsigned int adder, stopId;
    Direction dir;
    if(id == 0){
        current = trainStops[0];
        adder = 1;
        dir = Direction.Clockwise;
    } else {
        current = trainStops[(numStops+1)/2];
        adder = numStops-1;
        dir = Direction.CounterClockwise;
    }
    for(;;){
        try{
            _Accept(~Train){
                break;
            }
            or _Accept(embark){//stravation??
            }
            or _Accept(scanPassengers){
            }
            _Else{
                stopId = current->getId();
                prt.print(Printer::Kind::Train, id, 'A', stopId, numStudents);
                current->arrive(id, dir, maxNumStudents-numStudents);
                while(!stops.empty()){ //wake student to disembark
                    stops[stopId].signalBlock();
                }
            }
        }catch (uMutexFailure::RendezvousFailure &){
        }
        
    }
}