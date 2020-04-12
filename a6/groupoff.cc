#include "groupoff.h"
#include "printer.h"
#include "MPRNG.h"
#include "watcard.h"
#include <algorithm>

extern MPRNG mprng;

Groupoff::Groupoff(Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay)
    :prt{prt}, numStudents{numStudents}, maxTripCost{maxTripCost}, groupoffDelay{groupoffDelay}, counter{-1} {
        prt.print(Printer::Kind::Groupoff, 'S');
        futures = new WATCard::FWATCard[numStudents];
}


Groupoff::~Groupoff(){
    for(auto &c:futures){
        delete c;
    }
    delete[] futures;
    prt.print(Printer::Kind::Groupoff, 'F');
}

WATCard::FWATCard Groupoff::giftCard(){
    ++counter;
    return futures[counter];
}

void Groupoff::main(){
    for(unsigned int i=0; i<numStudents; ++i){
        _Accept(giftCard);
    }
    unsigned int lucky;
    for(unsigned int i=0; i<numStudents; ++i){
        _Accept(~Groupoff){
            break;
        }
        _Else{
            yield(groupoffDelay);
            WATCard* card = new WATCard();
            card->deposit(maxTripCost);
            lucky = mprng(counter); //random select
            std::swap(futures[lucky], futures[counter]);
            futures[counter].delivery(card);
            prt.print(Printer::Kind::Groupoff, 'D', maxTripCost);
            --counter;
        }
    }
}