#ifndef __PLAYER_H__
#define __PLAYER_H__

class Printer;
_Coroutine Player {
    // YOU MAY ADD PRIVATE MEMBERS, INCLUDING STATICS
    static unsigned int numPlayers;
    unsigned int id;
    unsigned int deck; //commuication
    Printer &printer;
    Player *lplayer;
    Player *rplayer;
    void main();
    void passDeck(unsigned int deck);
    _Event Schmilblick{};
    _Event GameOver{};
    static void terminate() {
        --Player::numPlayers;
    }
public:
    enum
    {
        DEATH_DECK_DIVISOR = 7
    };
    static void players(unsigned int num){
        numPlayers = num;
    }
    Player(Printer & printer, unsigned int id):
    id{id}, printer{printer}{
    }
    void start(Player & lp, Player & rp){
        lplayer = &lp;
        rplayer = &rp;
        resume();
    }
    void play(unsigned int deck){
        this->deck = deck;
        resume();
    }
    void drink(){
        // catch and throw exception
        resume();
    }
};

unsigned int Player::numPlayers;

#endif
