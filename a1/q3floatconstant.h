#ifndef FLOATCONSTANT_H
#define FLOATCONSTANT_H

_Coroutine FloatConstant{
public:
    enum Status {
        CONT,
        MATCH
    }; // possible status
private:
    char ch;
    Status status;
    void main(); // coroutine main
    void checkExp();
public:
    _Event Error{}; // last character is invalid
    Status next(char c) {
        ch = c;        // communication in
        resume();      // activate
        return status; // communication out
    }
    FloatConstant();
};
#endif