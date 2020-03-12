#include "q2printer.h"
#include <iostream>

Printer::Printer( unsigned int voters )
:Voter{voters}, Undefined{0}, cnt{0}, written{new bool[voters]} {
    for (auto &f:written){
        f = false;
    }
    int i=0;
    for(; i<voters-1; ++i){
        std::cout << 'V' << i << '\t';
    }
    std::cout << 'V' << i << std::endl;
    for(i=0 ; i<voters-1; ++i){
        std::cout << "*******" << '\t';
    }
    std::cout << "*******" << std::endl;
}

Printer::~Printer(){
    if(cnt > 0){
        flushBuffer();
    }
    std::cout << "*****************" << std::endl;
    std::cout << "All tours started" << std::endl;
    delete written;
}

void Printer::printItem(Items item){
    switch(item.state){
        case Voter::States::Start:
            std::cout << 'S';
            break;
        case Voter::States::Vote:
            std::cout << "V " << item.ballot.picture << ',' << item.ballot.statue << ',' << item.ballot.giftshop;
            break;
        case Voter::States::Block:
            std::cout << "B " << item.numBlocked;
            break;
        case Voter::States::Unblock:
            std::cout << "U " << item.numBlocked;
            break;
        case Voter::States::Barging:
            std::cout << "b " << item.numBlocked;
            break;
        case Voter::States::Done:
            std::cout << 'D';
            break;
        case Voter::States::Complete:
            std::cout << "C " << item.tour.tourkind;
            break;
        case Voter::States::Going:
            std:: cout << "G " << item.tour.tourkind << item.tour.groupno;
            break;
        case Voter::States::Failed:
            std::cout << 'X';
            break;
        case Voter::States::Terminated:
            std::cout << 'T';
            break;
    }
}

void Printer::flushBuffer(){
    for(int i=0; i<voters; ++i) {
        if(written[i]){
            printItem(columns[i]);
            --cnt;
            written[i] = false;
            if(cnt > 0) std::cout << '\t';
            else {
                std::cout << std::endl;
                break;
            }
        }
    }
}

void Printer::print( unsigned int id, Voter::States state ){
    curr = Items{id, state, undefined};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}
    
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot vote ){
    curr = Items{id, state, vote};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}
    
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){
    curr = Items{id, state, tour};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){
    curr = Items{id, state, numBlocked};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}