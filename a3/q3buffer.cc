#include "q3buffer.h"
#include "MPRNG.h"

MPRNG mprng = MPRNG();
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
    else clock.signal();
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
    else plock.signal();
    mutex.release();
    return item;
}
#endif // NOBUSY

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