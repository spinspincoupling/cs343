#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include "q2player.h"
#include "PRNG.h"
using namespace std:

int main( int argc, char * argv[] ) {
    int games = 5, seed = getpid();
    PRNG prng(seed);
    unsigned int players = prng(2,10);
    unsigned int cards = prng(10,200);
    try {                                               // process command-line arguments
        switch ( argc ) {
          case 5: if ( strcmp( argv[4], "d" ) != 0 ) {  // default ?
                seed = stoi( argv[4] ); if ( seed <= 0 ) throw 1;
            } // if
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                cards = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                players = stoi( argv[2] ); if ( players < 2 ) throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                games = stoi( argv[1] ); if ( games < 0 ) throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ games (>= 0) | d [ players (>= 2) | d"
             " [ cards (> 0) | d [ seed (> 0) | d ] ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    try {
        Player.players(players);
        while(games > 0) {
            Printer printer = Printer(players, cards);
            vector<Player> playerlist;
            for(unsigned int i=0; i<players; ++i) {
                playerlist.emplace_back(Player(printer), i);
            }
            for(unsigned int i=0; i<players; ++i) {
                playerlist[i].start(playerlist[(i+players-1)%players], playerlist[(i+1)%players]);
            }
            int start = prng(players-1);
            try{
                _Enable{
                    playerlist[start].play(cards);
                }
            }
            catch (...) { // one game terminates
                --games;
            }
            if(game > 0){
                cout<< endl <<endl;
            }
        }

    }