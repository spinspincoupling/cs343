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
            //std::cout << "catch at " << id << std::endl;
            printer.prt(id, 0, -1);
            _Resume Schmilblick() _At *rplayer;
            rplayer->drink();
            continue;
        }
        std::cout << "remaining players: " << numPlayers << std::endl;
        if(numPlayers == 1){ //win
            printer.prt(id, 0, numPlayers);
            _Resume GameOver() _At starter();
            return;
        }
        unsigned int take = prng(1, 8); 
        if(take >= deck) { //win
        std::cout << "should end" << std::endl;
            printer.prt(id, deck, numPlayers);
            _Resume GameOver() _At starter();
            return;
        }
        //std::cout << "take and not win at " << id << " count: " << take << std::endl; 
        printer.prt(id, take, numPlayers);
        if(deck%DEATH_DECK_DIVISOR == 0){ //check dead
            deck -= take;
            terminate();
            std::cout << "one die" << std::endl;
            break;
        }
        try {
            if(prng(9) == 0){
                //std:: cout << "start drink" <<" raise at " << id << std::endl;
                printer.prt(id, 0, -1);
                _Resume Schmilblick() _At *rplayer;
                rplayer->drink();
                _Enable{}
            }
        }
        catch (Schmilblick &){
            //std::cout << "return catch at " << id << std::endl;
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

