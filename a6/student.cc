#include "student.h"
#include "train.h"
#include <uFuture.h>

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
        unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips )
        :prt{prt}, nameServer{nameServer}, cardOffice{cardOffice}, Groupoff{Groupoff}, id{id},
        numStops{numStops}, stopCost{stopCost}, maxStudentDelay{maxStudentDelay}, maxStudentTrips{maxStudentTrips}{
        }

void Student::main(){
    unsigned int numTrips = mprng(1, maxStudentTrips);
    prt.print( Printer::Kind::Student, id, 'S', numTrips);
    unsigned int end = mprng(numStops-1), start, distance, cost;
    Train::Direction dir;
    bool buyTicket, useGiftcard, bought=false;
    WATCard::FWATCard watcard = cardOffice.create(id, maxTripCost); // called by student
    WATCard::FWATCard giftcard = groupoff.giftCard();
    TrainStop *stop = nameServer.getStop(id, end);
    try{
        for (int i=0; i< numTrips; ++i){
        yield(mprng(maxStudentDelay));
        start = end;
        end = (start + mprng(numStops-2))%numStops; 
        if(start > end){
            if(start - end < numStops-start+end){ // anti-clockwise
                dir = Train::Direction::CounterClockwise;
                distance = start-end;
            } else { //clockwise
                dir = Train::Direction::Clockwise;
                distance = numStops-start+end;
            }
        } else if(end - start > numStops-end+start){ // anti-clockwise
            dir = Train::Direction::CounterClockwise;
            distance = numStops-end+start;
        } else { //clockwise
            dir = Train::Direction::Clockwise;
            distance = end-start;
        }
        prt.print(Printer::Kind::Student, id, 'T', start, end, dir ==  Train::Direction::Clockwise? '<':'>');
        if(distance == 1){
            buyTicket = mprng(1) == 0? false:true;
        } else {
            buyTicket = mprng(9) < 3? false:true;
        }
        if(buyTicket){ // giftcard over watcard
            cost = distance*stopCost;
            if(giftcard.available()){
                useGiftcard = true;
                stop->buy(distance, *giftcard);
                prt.print(Printer::Kind::Student, id, cost, giftcard->getBalance());
                giftcard.reset();
            } else {
                useGiftcard = false;
                if(!watcard.available()) watcard();
                bought = false;
                try{
                    stop->buy(distance, *watcard);
                } catch(TrainStop::Funds &e) { //insufficent funds
                    watcard.reset();
                    watcard = cardOffice.transfer(id, maxTripCost+e.amount, &watcard);
                    stop->buy(distance, *(watcard()));
                } catch (WATCardOffice::Lost &){ //lost watcard in transfer
                    watcard.reset();
                    watcard = cardOffice.create();
                    prt.print(Printer::Kind::Student, id, 'L');
                    stop->buy(distance, *(watcard()));
                }
                prt.print(Printer::Kind::Student, id, cost, watcard->getBalance());
            }
        } else prt.print(Printer::Kind::Student, id, 'f');
        prt.print(Printer::Kind::Student, id, 'W', start);
        Train *train = stop->wait(id, dir);
        prt.print(Printer::Kind::Student, id, 'E', train->getId());
        stop = train->embark(id, end, useGiftcard? giftcard:watcard);
        prt.print(Printer::Kind::Student, id, 'D', end);
        stop->disembark(id);
    } catch(Train::Ejected &){ //terminate
        prt.print(Printer::Kind::Student, id, 'e');
    }
    prt.print(Printer::Kind::Student, id, 'F'); //terminate
    
}