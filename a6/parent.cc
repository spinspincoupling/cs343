#include "parent.h"
#include "printer.h"
#include "MPRNG.h"
#include "bank.h"

extern MPRNG mprng;

Parent::Parent( Printer & prt, Bank & bank, unsigned int numStudents, unsigned int parentalDelay, unsigned int maxTripCost )
    :prt{prt}, bank{bank}, numStudents{numStudents}, parentalDelay{parentalDelay}, maxTripCost{maxTripCost} {
}

Parent::~Parent(){
    prt.print(Printer::Kind::Parent, 'F');
}

void Parent::main(){
    prt.print(Printer::Kind::Parent, 'S');
    unsigned int student, amount;
    for(;;){
        _Accept(~Parent){
            break;
        }
        _Else{
            student = mprng(numStudents-1); //randomize student
            amount =  maxTripCost*mprng(1, 3)/3; //randomize amount
            amount = amount == 0? 1:amount; //run up if less than 1
            yield(parentalDelay);
            prt.print(Printer::Kind::Parent, 'D', student, amount);
            bank.deposit(student, amount);
        }
    }
}