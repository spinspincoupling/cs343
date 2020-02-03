#include <iostream>
#include "q2printer.h"

void Printer::clearBuffer(){
    for(int i=0; i<NoOfPlayers; ++i){
            if(!written[i]){
                std::cout << '\t';
                continue;
            }
            written[i] = false;
            if(remainingPlayers == -1){
                std::cout << 'D' << '\t';
                continue;
            }
            bool dead = (taken[i]+remainingCard[i]) % DEATH_DECK_DIVISOR == 0;
            bool win = remainingPlayers[i] == 1 || remainingCards[i] == 0;
            if(remainingPlayers[i] == 1) std::cout<< '#' << remainingCards[i];
            else std::cout << taken[i] << ':' << remainingCards[i];
            if(!win){
                if(remainingCards[i]%2 == 0) std::cout << '>';
                else std::cout << '<';
            } else std::cout << '#';
            if(dead) std::cout << 'X';
            std::cout << '\t';           
        }
}

Printer::Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards )
    :NoOfPlayers{NoOfPlayers}, NoOfCards{NoOfCards}{
        written = std::unique_ptr(new bool[NoOfPlayers]);
        taken = std::unique_ptr(new int[NoOfPlayers]);
        remainingCards = std::unique_ptr(new int[NoOfPlayers]);
        remainingPlayers = std::unique_ptr(new int[NoOfPlayers]);
        for(auto w:written) {
            w= false;
        }
        std::cout << "Players: " << NoOfPlayers <<  '\t' << "Cards: " << NoOfCards << std::endl;
        for(int i=0; i<NoOfPlayers; ++i){
            std::cout << "P" << i << '\t';
        }
        std::cout << std::endl;
}

Printer::~Printer(){
    clearBuffer();
}

void Printer::prt( unsigned int id, int took, int RemainingPlayers ){
    if(written[id]) {//clear buffer
        clearBuffer();
        std::cout << std::endl;
    }
    taken[id] = took;
    remainingPlayers[id] = remainingPlayers;
    NoOfCards -= took;
    remainingCards[id] = NoOfCards;
}