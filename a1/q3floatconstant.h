#ifndef FLOATCONSTANT_H
#define FLOATCONSTANT_H

_Coroutine FloatConstant{
public:
    enum Status {
        CONT,
        MATCH,
        INVALID
    }; // possible status
private:
    char ch;
    Status status;
    void main(); // coroutine main
public:
    _Event Error{}; // last character is invalid
    Status next(char c) {
        ch = c;        // communication in
        resume();      // activate
        return status; // communication out
    }
    Status getStatus(){
        return status;
    }
};
#endif