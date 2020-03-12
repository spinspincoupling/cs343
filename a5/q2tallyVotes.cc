#include "q2tallyVotes.h"
#include "q2voter.h"
#include "q2printer.h"
#include <climits>
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
    ++groupNum;
    kind = statues > pics && statues > shop? TourKind::Statue : pics > shop? TourKind::Picture : TourKind::GiftShop;
    pics = 0;
    statues = 0;
    shop = 0;
    formed = true;
}

#if defined( EXT )                    // mutex/condition solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}
    {}

    TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
        if(id == UINT_MAX) return;
        if(voters < group) throw Failed();
        PRINT(id, Voter::States::Vote, ballot);
        addVote(ballot);
        ++groupMem;
        if(groupMem < group){
            PRINT(id, Voter::States::Block, groupMem);
            _Accept(vote);
            PRINT(id, Voter::States::Unblock, groupMem);
            if(!formed) throw Failed();
            --groupMem;
            if(groupMem == 0) formed = false;
        } else {
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            --groupMem;
        }
        Tour tour = {kind, groupNum};
        return tour;

}

    void TallyVotes::done(){
        --voters;
        if(voters == group-1 && !formed){ // quorum failure
            this.vote(UINT_MAX, Ballot{0});
        }
    }

#elif defined( INT )                // semaphore solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, waiting{0}, formed{false} {
        grouping.P();
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        enterVote.P(); //barging avoidance
        mutex.P();
        if (voters < group) { // quorum failure
            enterVote.V();
            mutex.V();
            throw Failed();
        }
        PRINT(id, Voter::States::Vote, ballot);
        ++groupMem;
        addVote(ballot);
        if (groupMem == group) { //formed a group
            computeTour();
            groupMem = 0;
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
        } else {
            ++waiting;
            PRINT(id, Voter::States::Block, waiting);
            enterVote.V(); //allow other voters to enter to form a group
            mutex.V();
            grouping.P(); // waiting for a group
            mutex.P();
            --waiting;
            if (!formed) { // quorum failure
                if(waiting > 0) grouping.V();
                mutex.V();
                throw Failed();
            }
            PRINT(id, Voter::States::Unblock, waiting);
        }
        if(waiting == 0) { // let the new group come in
            enterVote.V();
            formed = false;
        }
        else grouping.V(); // let a group member come in
        Tour tour = Tour{kind, groupNum};
        mutex.V();
        return tour;
    }

    void TallyVotes::done(){
        mutex.P();
        --voters;
        if(voters == group-1){ // quorum failure
            if(!grouping.empty()){
                grouping.V();
            }
            if(!enterVote.empty()){
                enterVote.V();
            }
        }
        mutex.V();
    }

#elif defined( INTB )                // barrier solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :uBarrier(group), group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, waiting{0}, formed{false} 
    {}

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
        addVote(ballot);
        PRINT(id, Voter::States::Vote, ballot);
        if(uBarrier::waiters() < group-1){ // waiting for a complete group
            ++waiting;
            PRINT(id, Voter::States::Block, waiting);
            uBarrier::block();
            if(!formed){
                throw Failed();
            }
            --waiting;
            PRINT(id, Voter::States::Unblock, waiting);
            if(waiting == 0){
                formed = false; //reset for new group
                uBarrier::reset(group);
            }
        } else { // last one to form a complete group
            uBarrier::block();
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
        }
        Tour tour = Tour{kind, groupNum};
        return tour;
    }

    void TallyVotes::done(){
        --voters;
        if(voters < group && uBarrier::waiters() > 0){ //some one waiting for a group can never be formed
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }

#elif defined( AUTO )
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
    }

    void TallyVotes::done(){
        --voters;
        if(voters < group && uBarrier::waiters() > 0){ //some one waiting for a group can never be formed
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }

#elif defined( TASK )
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
    }

    void TallyVotes::done(){
        --voters;
        if(voters < group && uBarrier::waiters() > 0){ //some one waiting for a group can never be formed
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }
#else
    #error unsupported voter type
#endif