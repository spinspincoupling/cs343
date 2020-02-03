#include <iostream>
#include "q2printer.h"

void Printer::clearBuffer(){
    int lastwrite = -1;
    for(unsigned int i=0; i<NoOfPlayers; ++i){
            if(!written[i]){
                //if(i < NoOfPlayers-1) std::cout << '\t';
                continue;
            }
            lastwrite = i;
            written[i] = false;
            for(unsigned int j=lastwrite; j<i; ++j){
                std::cout << '\t';
            }
            if(remainingPlayers.get()[i] == -1){
                std::cout << 'D' ;
                if(i < NoOfPlayers-1) std::cout<< '\t';
                continue;
            }
            bool dead = (taken[i]+remainingCards[i]) % DEATH_DECK_DIVISOR == 0;
            bool win = remainingPlayers[i] == 1 || remainingCards[i] == 0;
            if(remainingPlayers[i] == 1) std::cout<< '#' << remainingCards[i];
            else std::cout << taken[i] << ':' << remainingCards[i];
            if(!win){
                if(remainingCards[i]%2 == 0) std::cout << '>';
                else std::cout << '<';
            } else std::cout << '#';
            if(dead) std::cout << 'X';
            if(i < NoOfPlayers-1)  std::cout << '\t';           
        }
    std::cout << std::endl;
}

Printer::Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards )
    :NoOfPlayers{NoOfPlayers}, NoOfCards{NoOfCards}, written{new bool[NoOfPlayers]},
    taken{new int[NoOfPlayers]}, remainingCards{new int[NoOfPlayers]},remainingPlayers{new int[NoOfPlayers]} {
        for(unsigned int i=0; i<NoOfPlayers; ++i) {
            written[i] = false;
        }
        std::cout << "Players: " << NoOfPlayers <<  "    " << "Cards: " << NoOfCards << std::endl;
        for(unsigned int i=0; i<NoOfPlayers; ++i){
            std::cout << "P" << i ;
            if(i < NoOfPlayers-1) std::cout << '\t';
        }
        std::cout << std::endl;
}

Printer::~Printer(){
    clearBuffer();
}

void Printer::prt( unsigned int id, int took, int RemainingPlayers ){
    if(written[id]) {//clear buffer
        clearBuffer();
    }
    written[id] = true;
    taken[id] = took;
    remainingPlayers[id] = RemainingPlayers;
    NoOfCards -= took;
    remainingCards[id] = NoOfCards;
}