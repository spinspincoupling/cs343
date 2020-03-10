#ifndef VOTER_H
#define VOTER_H
#include "q2tallyVotes.h"
class Printer;
_Task Voter {
    TallyVotes & voteTallier;
    Printer & printer;
    const unsigned int id;
    unsigned int nvotes;
    TallyVotes::Ballot cast();
    void main();
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
          Done = 'D', Complete = 'C', Going = 'G', Failed = 'X', Terminated = 'T' };
    Voter( unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer ):
    voteTallier{voteTallier}, printer{printer}, id{id}, nvotes{nvotes} {}
};

#endif