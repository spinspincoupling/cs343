#include "q2tallyVotes.h"
#if defined( BAR )
#include <uBarrier.h>
#endif
#if defined( SEM )
#include <uSemaphore.h>
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
    kind = statues > pics? TourKind::Statue : pics > shop? TourKind::Picture : TourKind::GiftShop;
    pics = 0;
    statues = 0;
    shop = 0;
    groupMem = 0;
}

#if defined( MC )                    // mutex/condition solution
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        mutex.acquire();
        if(voters < group) { // quorum failure
            mutex.release();
            throw Failed();
        }
        //std::cout << "first check " << signalled << std::endl;
        if(signalled > 0){ //barger
            ++barger;
            printer.print(id, Voter::States::Barging, barger);
            waitVote.wait(mutex);
            --barger;
            std::cout << "woke up from barger " << signalled << std::endl;
            --signalled;
            //std::cout << "wake from barger" << std::endl;
            if(voters < group) { // quorum failure
                mutex.release();
                throw Failed();
            }
        }
        printer.print(id, Voter::States::Vote, ballot);
        addVote(ballot);
        if(groupMem == group){ //formed a group
            computeTour();
            signalled += group-1;
            waitVoters.broadcast();
            //std::cout << "signal group " << signalled << std::endl;
            printer.print(id, Voter::States::Complete, Tour{kind, groupNum});
        } else {
            printer.print(id, Voter::States::Block, groupMem);
            ++waiting;
            waitVoters.wait(mutex);
            if(voters < group) { // quorum failure
                mutex.release();
                throw Failed();
            }
            --waiting;
            printer.print(id, Voter::States::Unblock, waiting);
            std::cout << "woke up from group " << signalled << std::endl;
            --signalled;
            //std::cout << "awake from wait vote " << signalled << std::endl;
        }
        if(waitVote.signal()) {++signalled;}
        Tour tour = {kind, groupNum};
        mutex.release();
        return tour;

    }

    void TallyVotes::done(){
        mutex.acquire();
        --voters;
        if(voters == group-1){ // quorum failure
            //groupMem = group;
            //std::cout << "detect failure" << std::endl;
            waitVoters.broadcast();
        }
        mutex.release();
    }
#elif defined( SEM )                // semaphore solution
    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        Tour tour = Tour(kind, groupNum);
        return tour;
    }

    void done(){
        mutex.P();
        --voters;
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