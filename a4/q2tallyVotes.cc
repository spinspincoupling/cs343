#include "q2tallyVotes.h"
#if defined( BAR )
#include <uBarrier.h>
#endif
#include "q2voter.h"
#include "printer.h"
#include <iostream>

void TallyVotes::addVote(Ballot ballot){
    ++groupMem;
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
    groupMem = 0;
}

#if defined( MC )                    // mutex/condition solution
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :groupMem{0}, waiting{0}, signalled{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, barger{0}
    {}

    TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
        mutex.acquire();
        if (voters < group) { // quorum failure
            mutex.release();
            throw Failed();
        }
        if (signalled > 0) { //barging prevention
            ++barger;
            printer.print(id, Voter::States::Barging, barger);
            waitVote.wait(mutex);
            --barger;
            --signalled;
            if (voters < group) { // quorum failure
                mutex.release();
                throw Failed();
            }
        }
        printer.print(id, Voter::States::Vote, ballot);
        addVote(ballot);
        if (groupMem == group) { //formed a group
            computeTour();
            signalled += group - 1;
            waitVoters.broadcast();
            printer.print(id, Voter::States::Complete, Tour{kind, groupNum});
        } else {
            printer.print(id, Voter::States::Block, groupMem);
            ++waiting;
            if (signalled == 0 && !waitVote.empty()) { // in case signal is lost
                waitVote.signal();
                ++signalled;
            }
            waitVoters.wait(mutex);
            if (voters < group) { // quorum failure
                mutex.release();
                throw Failed();
            }
            --waiting;
            printer.print(id, Voter::States::Unblock, waiting);
            --signalled;
        }
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
    :groupMem{0}, waiting{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, barger{0} {
        grouping.P();
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Ballot ballot ){
        enterVote.P();
        enterVote.P();
        if (voters < group) { // quorum failure
            if(!enterVote.empty()) enterVote.V();
            mutex.V();
            throw Failed();
        }
        printer.print(id, Voter::States::Vote, ballot);
        addVote(ballot);
        if (groupMem == group) { //formed a group
            computeTour();
            printer.print(id, Voter::States::Complete, Tour{kind, groupNum});
        } else {
            ++waiting;
            printer.print(id, Voter::States::Block, waiting);
            enterVote.V();
            mutex.V();
            grouping.P();
            mutex.P();
            --waiting;
            if (voters < group) { // quorum failure
                if(waiting > 0) grouping.V();
                mutex.V();
                throw Failed();
            }
            printer.print(id, Voter::States::Unblock, waiting);
        }
        --groupMem;
        if(groupMem == 0) vote.V();
        else grouping.V();
        Tour tour = Tour(kind, groupNum);
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
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        Tour tour = Tour(kind, groupNum);
        return tour;
    }

    void done(unsigned int id){
    }

#else
    #error unsupported voter type
#endif