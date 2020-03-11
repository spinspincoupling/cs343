#include "q2voter.h"
#include "q2tallyVotes.h"
#include "q2printer.h"
#include "MPRNG.h"
#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

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
        PRINT(id, States::Start);
        yield(mprng(4));
        TallyVotes::Tour tour = voteTallier.vote(id, cast());
        yield(mprng(4));
        PRINT(id, States::Going, tour);
        --nvotes;
        }
    }
    catch (TallyVotes::Failed &){
        PRINT(id, Failed);
    }
    #if defined( BAR )
    voteTallier.done(id);
    #else
    voteTallier.done();
    #endif
    PRINT(id, States::Terminated);
    
}