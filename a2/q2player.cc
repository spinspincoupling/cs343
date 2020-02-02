#include "q2player.h"
#include "q2printer.h"
#include "PRNG.h"

extern PRNG prng;

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
            printer.prt(id, -1, numPlayers);
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
            printer.prt(id, -1, numPlayers);
        }
        printer.prt(id, take, numPlayers); //see printer behaviors
        if(take >= deck || numPlayers == 1) { //win
            _Resume GameOver() _At starter();
        }
        deck -= take;
        if((deck + take)%DEATH_DECK_DIVISOR == 0){
            terminate();
            break;
        }
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

