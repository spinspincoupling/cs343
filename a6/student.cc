#include "student.h"
#include "groupoff.h"
#include "train.h"
#include "printer.h"
#include "nameserver.h"
#include "cardoffice.h"
#include "MPRNG.h"
#include "trainstop.h"
#include "global.h"
#include <uFuture.h>

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
    unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips )
    :prt{prt}, nameServer{nameServer}, cardOffice{cardOffice}, groupoff{groupoff}, id{id}, numStops{numStops}, 
    stopCost{stopCost}, maxStudentDelay{maxStudentDelay}, maxStudentTrips{maxStudentTrips}, numTrips{mprng(1, maxStudentTrips)} {
    prt.print( Printer::Kind::Student, id, 'S', numTrips);
}

void Student::main(){
    unsigned int end = mprng(numStops-1), start, distance, cost;
    Train::Direction dir;
    bool buyTicket;
    WATCard::FWATCard watcard = cardOffice.create(id, maxTripCost);
    WATCard::FWATCard giftcard = groupoff.giftCard();
    WATCard *cardUsing;
    TrainStop *stop;
    try{
        for(unsigned int i=0; i< numTrips; ++i){
            yield(mprng(maxStudentDelay));
            //determine start, end, direction
            start = end;
            end = (start + mprng(1,numStops-1))%numStops; 
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
            stop = nameServer.getStop(id, start);

            if(distance == 1){ //decide whether to pay
                buyTicket = mprng(1) == 0? false:true;
            } else {
                buyTicket = mprng(9) < 3? false:true;
            }
            if(!buyTicket) prt.print(Printer::Kind::Student, id, 'f');

            cost = distance*stopCost;
            for(;;){ //obtain watcard or giftcard
                try{
                    _When(buyTicket) _Select(giftcard){ //giftcard over watcard
                        cardUsing = giftcard;
                        stop->buy(distance, *cardUsing);
                        prt.print(Printer::Kind::Student, id, 'G', cost, cardUsing->getBalance());
                        giftcard.reset(); //deactivate after one use
                        break;
                    }
                    or _Select(watcard){
                        cardUsing = watcard;
                        if(buyTicket) {
                            stop->buy(distance, *cardUsing);
                            prt.print(Printer::Kind::Student, id, 'B', cost, cardUsing->getBalance());
                        }
                        break;
                    }
                }catch (TrainStop::Funds &e){ //not enough fund, need transfer
                    watcard = cardOffice.transfer(id, maxTripCost+e.amount, cardUsing); //can throw
                } catch (WATCardOffice::Lost &){
                    prt.print(Printer::Kind::Student, id, 'L');
                    watcard = cardOffice.create(id, maxTripCost);
                } 
            }
            prt.print(Printer::Kind::Student, id, 'W', start);
            Train *train = stop->wait(id, dir);
            prt.print(Printer::Kind::Student, id, 'E', train->getId());
            stop = train->embark(id, end, *cardUsing);
            stop->disembark(id);
            prt.print(Printer::Kind::Student, id, 'D', stop->getId());
        }
    }
    catch(Train::Ejected &){ //terminate
        prt.print(Printer::Kind::Student, id, 'e');
    }
    try{
        watcard();
        delete watcard; //wait for watcard. if succeed, delete heap allocated watcard
    } catch(...){ //in case lost
    }
    prt.print(Printer::Kind::Student, id, 'F'); //terminate
    
}

Student::~Student(){
}