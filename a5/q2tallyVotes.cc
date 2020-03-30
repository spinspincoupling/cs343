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
    :group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}, groupMem{0}
    {}

    TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
        if(voters < group) throw Failed();
        PRINT(id, Voter::States::Vote, ballot);
        addVote(ballot);
        ++groupMem;
        if(groupMem < group){
            PRINT(id, Voter::States::Block, groupMem);
            bool wait = true;
            while (wait && voters >= group){//wait for another vote
                try{
                    _Accept(vote){ //successfully accept one vote, can proceed
                        wait = false;
                    }
                    or _Accept(done){
                        PRINT(id, Voter::States::Done);
                    }     
                } catch (uMutexFailure::RendezvousFailure &){ //if signaller throw, it also throw
                    --groupMem;
                    PRINT(id, Voter::States::Unblock, groupMem);
                    throw Failed();
                }
            }
            --groupMem;
            PRINT(id, Voter::States::Unblock, groupMem);
            if(!formed) throw Failed(); // if wake up but group not formed, quorum failure
            if(groupMem == 0) formed = false;
        } else { // last one to form a group
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
    :group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}, groupMem{0} {
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){//no barger
        if(voters < group) throw Failed();
        PRINT(id, Voter::States::Vote, ballot);
        addVote(ballot);
        ++groupMem;
        if(groupMem < group){
            PRINT(id, Voter::States::Block, groupMem);
            grouping.wait(); //wait for more voters
            --groupMem;
            PRINT(id, Voter::States::Unblock, groupMem);
            if(!formed) { //wake up but group not formed, quorum failure
                if(groupMem > 0) grouping.signal(); //wake next onr to throw fail
                throw Failed();
            }
        } else {
            ++groupNum;
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            --groupMem;
        }
        if(groupMem > 0) grouping.signal(); //if there is someone waiting for voters, signal
        else formed = false; // reset
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
    :ticket{1}, voteWait{0}, barger{0}, group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{1}, formed{false}, groupMem{0} {
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
            while(ticketNum > groupNum){ //if wake up wrongly, blocked
                wait();
            }
            if(voters < group){ // if serving but could not form a group
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
            --voteWait;
            PRINT(id, Voter::States::Unblock, voteWait);
            if(!formed){
                throw Failed();
            }
        } else { // last one to form a complete group
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
            signalAll();
        }
        Tour tour = Tour{kind, groupNum};
        if(voteWait == 0){
            ++groupNum;
            formed = false; //reset for new group
            signalAll(); // wake next group
        }
        return tour;
    }

    void TallyVotes::done(){
        --voters;
        if(voters == group-1){ //quorum failure if there are waiting task
            signalAll();
        }
    }

#elif defined( AUTO )
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}, groupMem{0} {
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        if(voters < group){
            EXIT();
            throw Failed();
        }
        addVote(ballot);
        PRINT(id, Voter::States::Vote, ballot);
        if(groupMem < group-1){//need to wait for more voters
            ++groupMem;
            WAITUNTIL(formed || voters < group, PRINT(id, Voter::States::Block, groupMem) , --groupMem);
            PRINT(id, Voter::States::Unblock, groupMem); //wait until group formed or quorum failure
            if(!formed){ // if wake up because of quorum failure
                EXIT();
                throw Failed();
            } 
        } else { //formed group
            ++groupNum;
            computeTour();
            PRINT(id, Voter::States::Complete, Tour{kind, groupNum});
        }
        if(groupMem == 0){
            formed = false; //reset for new group
        }
        Tour tour = Tour{kind, groupNum};
        EXIT();
        return tour;
    }

    void TallyVotes::done(){
        --voters;
        EXIT();
    }

#elif defined( TASK )
    TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer &printer) 
    :group{group}, voters{voters}, printer{printer}, 
    pics{0}, statues{0}, shop{0}, groupNum{0}, formed{false}, groupMem{0} {
    }

    TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
        lastVoter = id; //set last voters
        if(voters < group){ //check quorum failure
            throw Failed();
        }
        addVote(ballot);
        PRINT(id, Voter::States::Vote, ballot);
        PRINT(id, Voter::States::Block, groupMem+1);
        voted.wait(); //wait for tallyvote to signal
        PRINT(id, Voter::States::Unblock, groupMem);
        if(!formed){ // if wake up due to quorum failure
            throw Failed();
        }
        Tour tour = Tour{kind, groupNum};
        return tour;
    }

    void TallyVotes::done(){
        --voters;
    }

    void TallyVotes::main(){
        for(;;){
            try{
                _Accept(done){ //prioritize done so avoid waking up task due to quorum failure
                    PRINT(lastVoter, Voter::States::Done); //wake up because of done
                    if(voters == group-1){
                        if(!voted.empty()) {
                            --groupMem;
                            voted.signalBlock();//to avoid infinite waiting, block itself
                        }
                    }
                } or _When(!formed) _Accept(vote){ //avoid barger
                    ++groupMem;
                    if(groupMem == group){//last vote forms a group
                        ++groupNum;
                        computeTour();
                        PRINT(lastVoter, Voter::States::Complete, Tour{kind, groupNum});
                        while(groupMem > 0){
                            --groupMem;
                            voted.signalBlock();//to avoid infinite waiting, block itself
                        }
                        formed = false;
                    }
                } or _Accept(~TallyVotes){//terminate if destructor call
                    break;
                } 
            } catch (uMutexFailure::RendezvousFailure &){//if one throw, any blocked task need to wake up
                if(!voted.empty()){
                    --groupMem;
                    voted.signalBlock();
                } 
            }
             
        }
        
    }
#else
    #error unsupported voter type
#endif