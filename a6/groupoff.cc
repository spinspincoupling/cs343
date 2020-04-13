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
        cards = new WATCard*[numStudents];
        for (unsigned int i=0; i<numStudents; ++i){
            list[i]=i;
            cards[i] = new WATCard();
        }
}


Groupoff::~Groupoff(){
    delete[] list;
    /*for(unsigned int i=0; i<numStudents; ++i){
        if(!futures[i].available()) futures[i].delivery(new WATCard());
    }*/
    for(unsigned int i=0; i<numStudents; ++i){
        delete cards[i];
    }
    delete[] cards;
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
            cards[counter]->deposit(maxTripCost);
            lucky = mprng(counter); //random select
            std::swap(list[lucky], list[counter]);
            futures[list[counter]].delivery(cards[counter]);
            prt.print(Printer::Kind::Groupoff, 'D', maxTripCost);
            --counter;
        }
    }
    prt.print(Printer::Kind::Groupoff, 'F');
}