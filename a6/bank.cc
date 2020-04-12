#include "bank.h"

Bank::Bank( unsigned int numStudents ):numStudents{numStudents}{
    accounts = new unsigned int[numStudents] {0};
    fund = new uCondition[numStudents];
}

Bank::~Bank(){
    delete[] accounts;
    delete[] fund;
}

Bank::void deposit( unsigned int id, unsigned int amount ){
    accounts[id] += amount;
    if(!fund[id].empty()) fund[id].signal();
}

Bank::void withdraw( unsigned int id, unsigned int amount ){
    while(accounts[id] < amount){
        fund[id].wait();
    }
    acounts[id] -= amount;
}