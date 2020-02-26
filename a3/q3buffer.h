#include <queue>
#include "MPRNG.h"

MPRNG mprng = MPRNG();
template<typename T> class BoundedBuffer {
    const unsigned int size;
    unsigned int buff;
    #ifdef NOBUSY 
    bool pwait;
    bool cwait;
    #endif
    std::queue<T> queue;
    uOwnerLock mutex;
    uCondLock plock;
    uCondLock clock;
  public:
    BoundedBuffer( const unsigned int size = 10 ): size{size}, buff{0}, pwait{false}, cwait{false} {}
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
    if(pwait || buff == size){
      plock.wait(mutex);
    }
    ++buff;
    queue.push(elem);
    if(clock.empty()) cwait = false;
    else {
      cwait = true;
      clock.signal();
    }
    mutex.release(); 
}

template<typename T>
T BoundedBuffer<T>::remove(){
    mutex.acquire();
    if(cwait || buff == 0){
        clock.wait(mutex);
    }
    --buff;
    T item = queue.front();
    queue.pop();
    if(plock.empty()) pwait = false;
    else {
      pwait = true;
      plock.signal();
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
    int subtotal = 0;
    for(;;){
        yield(mprng(Delay));
        int v = buffer.remove();
        if(v == Sentinel) break;
        subtotal += v;
    }
    sum = subtotal;
}
