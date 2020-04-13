#pragma once

#include <uFuture.h>

class WATCard {
    friend class TrainStop;
    
    unsigned int balance;
    bool hasPaid;

    WATCard( const WATCard & ) = delete;				// prevent copying
    WATCard & operator=( const WATCard & ) = delete;

    void markPaid();
    
  public:
    typedef Future_ISM<WATCard *> FWATCard;				// future watcard pointer
    WATCard();
    ~WATCard();
    void deposit( unsigned int amount );
    void withdraw( unsigned int amount );
    unsigned int getBalance();
    bool paidForTicket();
    void resetPOP();
}; // WATCard