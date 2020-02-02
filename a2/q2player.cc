#include <iostream>
#include "q2player.h"
#include "q2printer.h"
#include "PRNG.h"

extern PRNG prng;

unsigned int Player::numPlayers;

void Player::passDeck(unsigned int deck){
    if(deck%2 == 0) {
        rplayer->play(deck);
    } else {
        lplayer->play(deck);
    }
}

void Player::main(){
    suspend(); //start set cocaller
    //play get deck
    for(;;) {
        try {
                _Enable{}
        }
        catch (Schmilblick &){
            std::cout << "call to printer -1" << std::endl;
            printer.prt(id, 0, -1);
            _Resume Schmilblick() _At *rplayer;
            rplayer->drink();
        }
        unsigned int take = prng(1, 8); 
        if(take >= deck || numPlayers == 1) { //win
            printer.prt(id, take<deck? take:deck, numPlayers);
            int i = prng(9);
            _Resume GameOver() _At starter();
            return;
        }
        printer.prt(id, take, numPlayers);
        if(deck%DEATH_DECK_DIVISOR == 0){
            deck -= take;
            --numPlayers;
            break;
        }
        try {
            if(prng(9) == 0){
                std:: cout << "start drink" <<" count is " << take << std::endl;
                std::cout << "call to printer -1" << std::endl;
                printer.prt(id, 0, -1);
                _Resume Schmilblick() _At *rplayer;
                rplayer->drink();
                _Enable{}
            }
        }
        catch (Schmilblick &){
        }
        deck -= take;
        passDeck(deck);    
    }
    for(;;) { //dead
        try {
            _Enable{}
            passDeck(deck);
        }
        catch (Schmilblick &){
            _Resume Schmilblick() _At *rplayer;
            rplayer->drink();
        }
    }

}

