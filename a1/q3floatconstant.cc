#include "q3floatconstant.h"
#include <cctype>

const int maxDigits = 16;
const int maxExp = 3;

FloatConstant::FloatConstant(){
    status = CONT;
}

void FloatConstant::checkExp() {
    int exp = 0;
    if(ch == '+' || ch == '-') suspend();
    if(isdigit(ch)){
        ++exp;
        status = MATCH;
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
                    break;
                }
                else if (ch == 'E' || ch == 'e'){
                    suspend();
                    checkExp();
                }
                else throw Error();
                suspend();
            }
            if(digits > 1) status = MATCH;
            else {
                suspend();
                if(isdigit(ch)){
                    ++digits;
                    status = MATCH;
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
                status = CONT;
                suspend();
                checkExp();
            } else if(ch == 'f' || ch == 'l' || ch == 'F' || ch == 'L') {
                suspend(); // no more input beyond this point
                throw Error();
            } else throw Error();
        } else throw Error();
}