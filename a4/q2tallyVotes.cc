#include "q2tallyVotes.h"
#include "q2voter.h"
#include "printer.h"
#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

void TallyVotes::addVote(Ballot ballot){
    //++groupMem;
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
    //groupMem = 0;
    formed = true;
}

#if defined( MC )                    // mutex/condition solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, signalled{0}, barger{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, waiting{0}, formed{false}
    {}

    TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
        mutex.acquire();
        if (voters < group) { // quorum failure
            mutex.release();
            throw Failed();
        }
        if (signalled > 0) { //barging prevention
            ++barger;
            PRINT(id, Voter::States::Barging, barger);
            waitVote.wait(mutex);
            --barger;
            --signalled;
            if (voters < group) { // quorum failure
                mutex.release();
                throw Failed();
            }
        }
        PRINT(id, Voter::States::Vote, ballot);
        ++groupMem;
        addVote(ballot);
        if (groupMem == group) { //formed a group
            computeTour();
            groupMem = 0;
            signalled += group - 1;
            waitVoters.broadcast();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
        } else {
            PRINT(id, Voter::States::Block, groupMem);
            ++waiting;
            if (signalled == 0 && !waitVote.empty()) { // in case signal is lost
                waitVote.signal();
                ++signalled;
            }
            waitVoters.wait(mutex);
            if (!formed) { // quorum failure
                mutex.release();
                throw Failed();
            }
            --waiting;
            PRINT(id, Voter::States::Unblock, waiting);
            --signalled;
        }
        if(waiting == 0) formed = false;
        if (waitVote.signal()) {
            ++signalled;
        }
        Tour tour = {kind, groupNum};
        mutex.release();
        return tour;

}

    void TallyVotes::done(){
        mutex.acquire();
        --voters;
        if(voters == group-1){ // quorum failure
            waitVoters.broadcast();
        }
        mutex.release();
    }
#elif defined( SEM )                // semaphore solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, waiting{0}, formed{false} {
        grouping.P();
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        enterVote.P();
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
            enterVote.V();
            mutex.V();
            grouping.P();
            mutex.P();
            --waiting;
            if (!formed) { // quorum failure
                if(waiting > 0) grouping.V();
                mutex.V();
                throw Failed();
            }
            PRINT(id, Voter::States::Unblock, waiting);
        }
        if(waiting == 0) {
            enterVote.V();
            formed = false;
        }
        else grouping.V();
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

#elif defined( BAR )                // barrier solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :uBarrier(group), group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, waiting{0}, formed{false} 
    {}

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        if(voters < group){
            throw Failed();
        }
        addVote(ballot);
        PRINT(id, Voter::States::Vote, ballot);
        if(uBarrier::waiters() < group-1){
            ++waiting;
            PRINT(id, Voter::States::Block, waiting);
            uBarrier::block();
            if(!formed){
                throw Failed();
            }
            --waiting;
            PRINT(id, Voter::States::Unblock, waiting);
            if(waiting == 0){
                formed = false;
                uBarrier::reset(group);
            }
        } else {
            uBarrier::block();
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
        }
        Tour tour = Tour{kind, groupNum};
        return tour;
    }

    void TallyVotes::done(unsigned int id){
        --voters;
        if(voters < group && uBarrier::waiters() > 0){
            PRINT(id, Voter::States::Done);
            uBarrier::block();
        }
    }

#else
    #error unsupported voter type
#endif