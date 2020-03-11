#include "q2voter.h"
#include "q2tallyVotes.h"
#include "printer.h"
#include "MPRNG.h"

extern MPRNG mprng;

TallyVotes::Ballot cast() {        // cast 3-way vote
        // O(1) random selection of 3 items without replacement using divide and conquer.
        static const unsigned int voting[3][2][2] = { { {2,1}, {1,2} }, { {0,2}, {2,0} }, { {0,1}, {1,0} } };
        unsigned int picture = mprng( 2 ), statue = mprng( 1 );
        return (TallyVotes::Ballot){ picture, voting[picture][statue][0], voting[picture][statue][1] };
    }

void Voter::main(){
    try {
        while(nvotes > 0){
        yield(mprng(19));
        printer.print(id, States::Start);
        yield(mprng(4));
        TallyVotes::Tour tour = voteTallier.vote(id, cast());
        yield(mprng(4));
        printer.print(id, States::Going, tour);
        --nvotes;
        }
    }
    catch (TallyVotes::Failed &){
        printer.print(id, Failed);
    }
    #if defined( BAR )
    voteTallier.done(id);
    #else
    voteTallier.done();
    #endif
    printer.print(id, States::Terminated);
    
}