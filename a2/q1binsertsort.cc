#include "q1binsertsort.h"

template<typename T>
void Binsertsort<T>::main(){
    try{
        _Enable{}
    }
    catch (Sentinel &){
        _Resume Sentinel() _At resumer();
    }
    T pivot = value;
    try {
        suspend();
         _Enable{}
    }
    catch (Sentinel &) { // end of value set
        suspend();
        _Resume Sentinel() _At resumer();
    }
    // implies vertex node
    Binsertsort<T> less, greater; // create less and greater
    try{
        for (;;) {
            if (value < pivot) {
                less.sort(value);
            } else {
                greater.sort(value);
            }
            suspend();
            _Enable{}
        }
    }
    catch (Sentinel &) { // end of value set, first retrive
    }
    _Resume Sentinel() _At less;
    try{
        for (;;) {
            _Enable{
                value = less.retrieve();
            }
            suspend();
        }
    }
    catch (Sentinel &) { //exception from left child end
    }
    value = pivot;
    suspend();
    _Resume Sentinel() _At greater;
    try {
        for (;;) {
            _Enable {
                value = greater.retrieve();
            }
            suspend();
        }
    }
    catch (Sentinel &){ // all branch ends notify parent
    }
    _Resume Sentinel() _At resumer();
}