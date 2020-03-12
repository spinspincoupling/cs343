#include "q2printer.h"
#include <iostream>
#include <climits>

Printer::Printer( unsigned int voters )
:Voters{voters}, Undefined{UINT_MAX}, cnt{0}, written{new bool[voters]}, columns{new Items[voters]} {
    for (unsigned int i=0; i<voters; ++i){
        written[i] = false;
    }
    unsigned int i=0;
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
    delete columns;
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
            std::cout << "C " << static_cast<char>(item.tour.tourkind);
            break;
        case Voter::States::Going:
            std:: cout << "G " << static_cast<char>(item.tour.tourkind) << ' ' << item.tour.groupno;
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
    for(unsigned int i=0; i<Voters; ++i) {
        if(written[i]){
            printItem(columns[i]);
            --cnt;
            written[i] = false;
            if(cnt == 0) {
                std::cout << std::endl;
                break;
            }
        } else std::cout << '\t';
    }
}

void Printer::print( unsigned int id, Voter::States state ){
    curr = Items{id, state, .numBlocked=Undefined};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}
    
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot vote ){
    curr = Items{id, state, .ballot=vote};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}
    
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){
    curr = Items{id, state, .tour=tour};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){
    curr = Items{id, state, .numBlocked=numBlocked};
    if(written[id]){
        flushBuffer();
    }
    columns[id] = curr;
    written[id] = true;
    ++cnt;
}