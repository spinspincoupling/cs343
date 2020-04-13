#include "watcard.h"
#include <iostream>

WATCard::WATCard()
    :balance{0}, hasPaid{false} {
}

WATCard::~WATCard(){
}

void WATCard::deposit( unsigned int amount ){
    balance += amount;
}

void WATCard::withdraw( unsigned int amount ){
    balance -= amount;
}

unsigned int WATCard::getBalance(){
    return balance;
}

bool WATCard::paidForTicket(){
    return hasPaid;
}

void WATCard::resetPOP(){
    hasPaid = false;
    std::cout << "reset" << '\n';
}

void WATCard::markPaid(){
    hasPaid = true;
    std::cout << "mark paid" << '\n';
}