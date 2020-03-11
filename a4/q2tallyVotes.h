#ifndef TALLYVOTE_H
#define TALLYVOTE_H

class Printer;
#if defined( MC )                    // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
    uOwnerLock mutex;
    uCondLock waitVoters;
    uCondLock waitVote;
    unsigned int groupMem;
    unsigned int waiting;
    unsigned int signalled;
#elif defined( SEM )                // semaphore solution
class uSemaphore;
class TallyVotes {
    // private declarations for this kind of vote-tallier
    uSemaphore & mutex;
    uSemaphore & grouping;
    uSemaphore & vote;
    unsigned int groupMem;
    unsigned int waiting;
#elif defined( BAR )                // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    const unsigned int group;
    unsigned int voters;
    Printer &printer;
    unsigned int pics, statues, shop;
    unsigned int groupNum;
    unsigned barger;
  public:                            // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum TourKind { Picture = 'p', Statue = 's', GiftShop = 'g' };
    struct Tour { TourKind tourkind; unsigned int groupno; };
    Tour vote( unsigned int id, Ballot ballot );
    void done(
    #if defined( BAR )               // barrier solution
        unsigned int id
    #endif
    );
  private:
    TourKind kind;
    void addVote(Ballot ballot);
    void computeTour();
};

#endif