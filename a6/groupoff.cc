#include "groupoff.h"
#include "printer.h"
#include "MPRNG.h"
#include "watcard.h"
#include <algorithm>
#include <iostream>

extern MPRNG mprng;

Groupoff::Groupoff(Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay)
    :prt{prt}, numStudents{numStudents}, maxTripCost{maxTripCost}, groupoffDelay{groupoffDelay}, counter{-1} {
        prt.print(Printer::Kind::Groupoff, 'S');
        futures = new WATCard::FWATCard[numStudents];
        list = new unsigned int[numStudents];
        for (unsigned int i=0; i<numStudents; ++i){
            list[i]=i;
        }

}


Groupoff::~Groupoff(){
    /*for(unsigned int i=0; i<numStudents; ++i){
        futures[i]();
    }*/
    delete[] list;
    for(unsigned int i=0; i<numStudents; ++i){
        delete futures[i];
    }
    delete[] futures;
    
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
            std::swap(list[lucky], list[counter]); //cannot because copy!!
            futures[list[counter]].delivery(card);
            prt.print(Printer::Kind::Groupoff, 'D', maxTripCost);
            --counter;
        }
    }
    prt.print(Printer::Kind::Groupoff, 'F');
}