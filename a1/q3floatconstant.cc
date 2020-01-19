#include "q3floatconstant.h"
#include <cctype>

const int maxDigits = 16;
const int maxExp = 3;

FloatConstant::FloatConstant() override{
    status = Status.CONT;
}

void checkExp() {
    int exp = 0;
    if(ch == '+' || ch == '-') suspend();
    if(isdigit(ch)){
        ++exp;
        status = Status.MATCH;
        suspend();
        for (;;) {
            if(isdigit(ch)) {
                ++exp;
                if(exp > maxExp) throw Error();
            } else if(ch == 'f' || ch == 'l' || ch == 'F' || ch == 'L') {
                suspend(); // no more input beyond this point
                throw Error();
            } else throw Error();
            suspend();
        }
    }
    throw Error();
}

void FloatConstant::main(){
    int digits = 0; 
    try{
        if(ch == '+' || ch == '-'){
            suspend();
        }
        if (isdigit(ch) || ch == '.'){
            for (;;){
                if (isdigit(ch)){
                    ++digits;
                    if (digits > maxDigits) throw Error();
                }
                else if (ch == '.'){
                    ++digits;
                    if (digits > maxDigits) throw Error();
                    break;
                }
                else if (ch == 'E' || ch == 'e'){
                    status = Status.CONT;
                    suspend();
                    checkExp();
                }
                else throw Error();
                suspend();
            }
        }
        if(digits > 1) status = Status.MATCH;
        else {
            suspend();
            if(isdigit(ch)){
                ++digits;
                status = Status.MATCH;
            } else throw Error();
        }
        suspend();
        for(;;){
            if(isdigit(ch)){
                ++digits;
                if(digits > maxDigits) throw Error();
            } else break;
            suspend();
        }
        if(ch == 'E' || ch == 'e'){
            status = Status.CONT;
            suspend();
            checkExp();
        } else if(ch == 'f' || ch == 'l' || ch == 'F' || ch == 'L') {
            suspend(); // no more input beyond this point
            throw Error();
        } else throw Error();

    } catch (Error){
        status = Status.INVALID;
    }
}