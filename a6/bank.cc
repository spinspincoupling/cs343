#include "bank.h"

Bank::Bank( unsigned int numStudents ):numStudents{numStudents}{
    accounts = new unsigned int[numStudents] {0};
    fund = new uCondition[numStudents];
}

Bank::~Bank(){
    delete[] accounts;
    delete[] fund;
}

void Bank::deposit( unsigned int id, unsigned int amount ){
    accounts[id] += amount;
    if(!fund[id].empty()) fund[id].signal(); // if some one waiting for fund
}

void Bank::withdraw( unsigned int id, unsigned int amount ){
    while(accounts[id] < amount){ //wait till have enough fund
        fund[id].wait();
    }
    accounts[id] -= amount;
}