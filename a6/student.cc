#include "student.h"
#include "groupoff.h"
#include "train.h"
#include "printer.h"
#include "nameserver.h"
#include "cardoffice.h"
#include "MPRNG.h"
#include "trainstop.h"
#include <uFuture.h>
#include <iostream>

//Student::CreateWatcard::CreateWatcard(Printer & prt, WATCardOffice & cardOffice):
//    prt{prt}, cardOffice{cardOffice}{}
//
//void Student::CreateWatcard::main(){
//    WATCard::FWATCard watcard = cardOffice.create(id, maxTripCost);
//    for(;;){
//        _Accept(~CreateWatcard){
//            break;
//        }
//        or _Accept(getWatcard){
//            avail.signalBlocked();
//        }
//    }
//}

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
    unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips )
    :prt{prt}, nameServer{nameServer}, cardOffice{cardOffice}, groupoff{groupoff}, id{id},
    numStops{numStops}, stopCost{stopCost}, maxStudentDelay{maxStudentDelay}, maxStudentTrips{maxStudentTrips}{
    }

void Student::main(){
    unsigned int numTrips = mprng(1, maxStudentTrips);
    prt.print( Printer::Kind::Student, id, 'S', numTrips);
    unsigned int end = mprng(numStops-1), start, distance, cost;
    Train::Direction dir;
    bool buyTicket, getcard, error = false;
    WATCard::FWATCard watcard;
    WATCard::FWATCard giftcard = groupoff.giftCard();
    WATCard *cardUsing;
    TrainStop *stop = nameServer.getStop(id, end);
    try{
        watcard = cardOffice.create(id, maxTripCost);
        //if(resumed) std::cout << "pass create" << '\n';
        for(unsigned int i=0; i< numTrips; ++i){
            yield(mprng(maxStudentDelay));
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
            //if(resumed) std::cout << "pass determine dir" << '\n';
            prt.print(Printer::Kind::Student, id, 'T', start, end, dir ==  Train::Direction::Clockwise? '<':'>');
            if(distance == 1){
                buyTicket = mprng(1) == 0? false:true;
            } else {
                buyTicket = mprng(9) < 3? false:true;
            }
            if(buyTicket){ // giftcard over watcard
                cost = distance*stopCost;
                //if(resumed) std::cout << "before get card" << '\n';
                if(giftcard.available()){
                    cardUsing = giftcard;
                    //getcard = true;
                    stop->buy(distance, *cardUsing);
                    prt.print(Printer::Kind::Student, id, 'G', cost, cardUsing->getBalance());
                    giftcard.reset();
                } else {
                    getcard = false;
                    while(!getcard){
                        //std::cout << "first attempt to access" << '\n';
                        cardUsing = watcard();
                        if(!error) getcard = true;
                        else {
                            watcard.reset();
                            watcard = cardOffice.create(id, maxTripCost); //can throw
                            error = false;
                        }
                    }
                    try{
                            //if(resumed) std::cout << "before buy" << '\n';
                            stop->buy(distance, *cardUsing);
                    } catch(TrainStop::Funds &e) { //insufficent funds
                                watcard.reset();
                                watcard = cardOffice.transfer(id, maxTripCost+e.amount, watcard); //can throw
                                cardUsing = watcard();
                                stop->buy(distance, *cardUsing);
                    } catch (WATCardOffice::Lost &){ //lost watcard in transfer
                                getcard = false;
                                while(!getcard) {
                                    try {
                                        watcard.reset();
                                        prt.print(Printer::Kind::Student, id, 'L');
                                        watcard = cardOffice.create(id, maxTripCost); //can throw
                                        cardUsing = watcard();
                                        getcard = true;
                                    } catch(WATCardOffice::Lost &){
                                    }
                                }
                                stop->buy(distance, *cardUsing);
                            }
                            prt.print(Printer::Kind::Student, id, 'B', cost, cardUsing->getBalance());
                }
            } else {
                prt.print(Printer::Kind::Student, id, 'f');
                while(!getcard){
                    cardUsing = watcard();
                    if(!error) getcard = true;
                    else {
                        watcard.reset();
                        watcard = cardOffice.create(id, maxTripCost); //can throw
                        error = false;
                    }
                }
            }
            prt.print(Printer::Kind::Student, id, 'W', start);
            Train *train = stop->wait(id, dir);
            prt.print(Printer::Kind::Student, id, 'E', train->getId());
            stop = train->embark(id, end, *cardUsing);
            prt.print(Printer::Kind::Student, id, 'D', end);
            stop->disembark(id);
            //if(resumed) std::cout << "before set resume to false" << '\n';
            //resumed = false;
        }
        //delete watcard;
        //delete giftcard;
    } 
    _CatchResume(WATCardOffice::Lost &){
        error = true;
        //std::cout << "outer lost" << '\n';
        prt.print(Printer::Kind::Student, id, 'L');
    }
     catch(Train::Ejected &){ //terminate
        prt.print(Printer::Kind::Student, id, 'e');
    }
    prt.print(Printer::Kind::Student, id, 'F'); //terminate
    
}

Student::~Student(){
}