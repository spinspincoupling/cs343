#ifndef TALLYVOTE_H
#define TALLYVOTE_H

class Printer;

#if defined( EXT )                           // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
#elif defined( INT )                         // internal scheduling monitor solution
// includes for this kind of vote-tallier   
_Monitor TallyVotes {
    uCondition grouping;
#elif defined( INTB )                        // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    unsigned int ticket;
    unsigned int voteWait;
    unsigned int barger;
    uCondition bench;                        // only one condition variable (variable name may be changed)
    void wait();                             // barging version of wait
    void signalAll();                        // unblock all waiting tasks
#elif defined( AUTO )                        // automatic-signal monitor solution
#include "AutomaticSignal.h"
_Monitor TallyVotes {
    AUTOMATIC_SIGNAL;
#elif defined( TASK )                        // internal/external scheduling task solution
_Task TallyVotes {
    uCondition voted;
    unsigned int lastVoter;
    void main();
#else
    #error unsupported voter type
#endif
    const unsigned int group;
    unsigned int voters;
    Printer &printer;
    unsigned int pics, statues, shop;
    unsigned int groupNum;
    bool formed;
    unsigned int groupMem;
  public:                                    // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum TourKind { Picture = 'p', Statue = 's', GiftShop = 'g' };
    struct Tour { TourKind tourkind; unsigned int groupno; };
    Tour vote( unsigned int id, Ballot ballot );
    void done();
  private:
    TourKind kind;
    void addVote(Ballot ballot);
    void computeTour();
};

#endif