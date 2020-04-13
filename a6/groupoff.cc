#include "groupoff.h"
#include "printer.h"
#include "MPRNG.h"
#include "watcard.h"
#include <algorithm>

extern MPRNG mprng;

Groupoff::Groupoff(Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay)
    :prt{prt}, numStudents{numStudents}, maxTripCost{maxTripCost}, groupoffDelay{groupoffDelay}, counter{-1} {
        prt.print(Printer::Kind::Groupoff, 'S');
        futures = new WATCard::FWATCard[numStudents]; //futures to be returned to students
        cards = new WATCard*[numStudents]; 
        remaining = new unsigned int[numStudents]; //for the random draw
        for (unsigned int i=0; i<numStudents; ++i){
            remaining[i]=i;
            cards[i] = new WATCard(); //prepare all watcards
        }
}


Groupoff::~Groupoff(){
    delete[] remaining;
    for(unsigned int i=0; i<numStudents; ++i){
        delete cards[i];
    }
    delete[] cards;
    delete[] futures;
    
}

WATCard::FWATCard Groupoff::giftCard(){
    ++counter; //counter is initialized to -1 so that it reflect the index
    return futures[counter]; //assign a future
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
            std::swap(remaining[lucky], remaining[counter]); //deactivate the index
            futures[remaining[counter]].delivery(cards[counter]);
            prt.print(Printer::Kind::Groupoff, 'D', maxTripCost);
            --counter;
        }
    }
    prt.print(Printer::Kind::Groupoff, 'F');
}