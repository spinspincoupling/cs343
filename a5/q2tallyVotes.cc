#include "q2tallyVotes.h"
#include "q2voter.h"
#include "q2printer.h"
#include <iostream>
#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

void TallyVotes::addVote(Ballot ballot){
    pics += ballot.picture;
    statues += ballot.statue;
    shop += ballot.giftshop;
}

void TallyVotes::computeTour(){
    kind = statues > pics && statues > shop? TourKind::Statue : pics > shop? TourKind::Picture : TourKind::GiftShop;
    pics = 0;
    statues = 0;
    shop = 0;
    formed = true;
}

#if defined( EXT )                    // external scheduling solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}
    {}

    TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
        if(voters < group) throw Failed();
        PRINT(id, Voter::States::Vote, ballot);
        addVote(ballot);
        ++groupMem;
        if(groupMem < group){
            PRINT(id, Voter::States::Block, groupMem);
            bool wait = true;
            while (wait && voters >= group){
                try{
                    _Accept(vote){
                        wait = false;
                    }
                    or _Accept(done){
                        PRINT(id, Voter::States::Done);
                    }     
                } catch (uMutexFailure::RendezvousFailure &){
                    throw Failed();
                }
            }
            --groupMem;
            PRINT(id, Voter::States::Unblock, groupMem);
            if(!formed) throw Failed();
            if(groupMem == 0) formed = false;
        } else {
            ++groupNum;
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            --groupMem;
        }
        Tour tour = {kind, groupNum};
        return tour;

}

    void TallyVotes::done(){
        --voters;
    }

#elif defined( INT )                // internal scheduling solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false} {
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        if(voters < group) throw Failed();
        PRINT(id, Voter::States::Vote, ballot);
        addVote(ballot);
        ++groupMem;
        if(groupMem < group){
            PRINT(id, Voter::States::Block, groupMem);
            grouping.wait();
            --groupMem;
            PRINT(id, Voter::States::Unblock, groupMem);
            if(!formed) {
                if(groupMem > 0) grouping.signal();
                throw Failed();
            }
        } else {
            ++groupNum;
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            --groupMem;
        }
        if(groupMem > 0) grouping.signal();
        else formed = false;
        Tour tour = Tour{kind, groupNum};
        return tour;
    }

    void TallyVotes::done(){
        --voters;
        if(voters == group-1){ // may be quorum failure
            grouping.signal();
        }
    }

#elif defined( INTB )
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, ticket{0}, voteWait{0}, barger{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false} {
    }

    void TallyVotes::wait() {
        bench.wait();                            // wait until signalled
        while ( rand() % 2 == 0 ) {              // multiple bargers allowed
            try {
                _Accept( vote, done ) {          // accept barging callers
                } _Else {                        // do not wait if no callers
                } // _Accept
            } catch( uMutexFailure::RendezvousFailure & ) {}
        } // while
    }

    void TallyVotes::signalAll() {               // also useful
        while ( ! bench.empty() ) bench.signal();// drain the condition
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        if(voters < group){
            throw Failed();
        }
        unsigned int ticketNum = ticket;
        if(groupMem == group-1){
            groupMem = 0;
            ++ticket;
        } else {
            ++groupMem;
        }
        if(ticketNum > groupNum){// not serving this group, barger
            ++barger;
            PRINT(id, Voter::States::Barging, barger);
            while(ticketNum > groupNum){
                wait();
            }
            if(voters < group){
                throw Failed();
            }
            --barger;
        }
        addVote(ballot);
        PRINT(id, Voter::States::Vote, ballot);
        if(voteWait < group-1){ // waiting for a complete group
            ++voteWait;
            PRINT(id, Voter::States::Block, voteWait);
            wait();
            if(!formed){
                throw Failed();
            }
            --voteWait;
            PRINT(id, Voter::States::Unblock, voteWait);
        } else { // last one to form a complete group
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            signalAll();
        }
        if(voteWait == 0){
            ++groupNum;
            formed = false; //reset for new group
            signalAll(); // wake next group
        }
        Tour tour = Tour{kind, groupNum};
        return tour;
    }

    void TallyVotes::done(){
        --voters;
        if(voters == group-1){ //quorum failure if there are waiting task
            signalAll();
        }
    }

#elif defined( AUTO )
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
    }

    void TallyVotes::done(){
        --voters;
        if(voters < group){ //some one waiting for a group can never be formed
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }

#elif defined( TASK )
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
    }

    void TallyVotes::done(){
        --voters;
        if(voters < group){ //some one waiting for a group can never be formed
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }
#else
    #error unsupported voter type
#endif