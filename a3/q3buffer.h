#ifndef _BUFFER_H
#define _BUFFER_H
#include <queue>
#include "MPRNG.h"

MPRNG mprng = MPRNG();
template<typename T> class BoundedBuffer {
    const unsigned int size;
    unsigned int buff;
    #ifdef NOBUSY
    bool busy;
    uCondLock block;
    #endif
    std::queue<T> queue;
    uOwnerLock mutex;
    uCondLock plock;
    uCondLock clock;
  public:
  #ifdef BUSY 
    BoundedBuffer( const unsigned int size = 10 ): size{size}, buff{0} {}
  #endif
  #ifdef NOBUSY
    BoundedBuffer( const unsigned int size = 10 ): size{size}, buff{0}, busy{false} {}
  #endif
    void insert( T elem );
    T remove();
};

#ifdef BUSY                            // busy waiting implementation
template<typename T>
void BoundedBuffer<T>::insert( T elem ){
    mutex.acquire();
    while(buff == size){
        plock.wait(mutex);
    }
    ++buff;
    queue.push(elem);
    clock.signal();
    mutex.release();
}

template<typename T>
T BoundedBuffer<T>::remove(){
    mutex.acquire();
    while(buff == 0){
        clock.wait(mutex);
    }
    --buff;
    T item = queue.front();
    queue.pop();
    plock.signal();
    mutex.release();
    return item;
}
#endif // BUSY

#ifdef NOBUSY                          // no busy waiting implementation
template<typename T>
void BoundedBuffer<T>::insert( T elem ){
    mutex.acquire();
    if (busy) {
        block.wait(lock);
        if (block.empty()) {
            busy = false;
        }
    }
    if (buff == size) {
        block.signal(); // one in one out
        plock.wait(mutex);
    }
    //if(buff == size || pwait > 0){
      //plock.wait(mutex);
    //}
    ++buff;
    queue.push(elem);
    //if(clock.empty()) cwait = false;
    //else {
      //clock.signal();
    //}
    busy = !block.empty() || !clock.empty();
    if (!clock.empty()) {
        clock.signal();
    } else if (!block.empty()) {
        block.signal();
    }
    mutex.release(); 
}

template<typename T>
T BoundedBuffer<T>::remove(){
    mutex.acquire();
    //if(buff == 0 ){
    //    clock.wait(mutex);
    //}
    if (busy) {
        block.wait(lock);
        if (block.empty()) {
            busy = false;
        }
    }
    if (buff == 0) {
        block.signal(); // one in one out
        clock.wait(mutex);
    }
    --buff;
    T item = queue.front();
    queue.pop();
    //if(plock.empty()) pwait = false;
    //else {
      //pwait = true;
      //plock.signal();
    //}
    busy = !block.empty() || !plock.empty();
    if (!plock.empty()) {
        plock.signal();
    } else if (!block.empty()) {
        block.signal();
    }
    mutex.release();
    return item;
}
#endif // NOBUSY

_Task Producer {
    BoundedBuffer<int> &buffer;
    const int Produce;
    const int Delay;
    void main();
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay )
    :buffer{buffer}, Produce{Produce}, Delay{Delay} {}
};

_Task Consumer {
    BoundedBuffer<int> &buffer;
    const int Delay;
    const int Sentinel;
    int &sum;
    void main();
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum )
    :buffer{buffer}, Delay{Delay}, Sentinel{Sentinel}, sum{sum} {}
};
void Producer::main(){
    for(int i=1; i<=Produce; ++i){
        yield(mprng(Delay));
        buffer.insert(i);
    }  
}

void Consumer::main(){
    for(;;){
        yield(mprng(Delay));
        int v = buffer.remove();
        if(v == Sentinel) break;
        sum += v;
    }
}

#endif
