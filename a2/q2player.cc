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
            printer.prt(id, -1, 0);
            _Resume Schmilblick() _At *rplayer;
            rplayer->drink();
        }
        unsigned int take = prng(1, 8); 
        try {
            if(prng(9) == 0){
                _Resume Schmilblick() _At *rplayer;
                rplayer->drink();
                _Enable{}
            }
        }
        catch (Schmilblick &){
            printer.prt(id, -1, 0);
        }
        if(take >= deck || numPlayers == 1) { //win
            printer.prt(id, take, numPlayers);
            _Resume GameOver() _At starter();
            return;
        }
        deck -= take;
        if((deck + take)%DEATH_DECK_DIVISOR == 0){
            --numPlayers;
            std::cout << id << " " << take <<" " << deck << " " << numPlayers << std::endl;
            printer.prt(id, take, numPlayers);
            break;
        }
        printer.prt(id, take, numPlayers);
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

