#include "q2polymult.h"
#include <iostream>
#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

void calculate(const poly_t & a, const poly_t & b, poly_t & c, const size_t delta, size_t start){
    size_t index=start; 
        while(index < c.size){
            int total = 0;
            for(unsigned int i=index<a.size? 0: index+1-a.size; i<a.size; ++i){
                //if(i+a.size-1 < index) continue;
                if(i > index) break;
                total += a.arr[i]*b.arr[index-i];
            }
            c.arr[index] = total;
            index += delta;
        }
}

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta ) {

#if defined( CFOR )
    COFOR(s, 0, delta,
        calculate(a, b, c, delta, s);
	);
    
#elif defined( ACTOR )
    struct WorkMsg : public uActor::Message {
	size_t delta;
    size_t start;
    const poly_t & a;
    const poly_t & b;
    poly_t & c;
	WorkMsg(size_t delta, size_t start, const poly_t & a, const poly_t & b, poly_t & c) : 
    Message( uActor::Delete ), delta{delta}, start{start}, a{a}, b{b}, c{c} {}
    };
    _Actor Multiply {
	Allocation receive( Message & w ) {
	    Case( WorkMsg, w ) {			// discriminate derived message
            calculate(w_d->a, w_d->b, w_d->c, w_d->delta, w_d->start);
	    } else assert( false );			// bad message
	    return Finished;				// one-shot
	} // Multiply::receive
    }; // Multiply

    uActorStart();					// start actor system
    Multiply m[delta];
    for(unsigned int i=0; i<delta; ++i){
        m[i] | *new WorkMsg(delta, i, a, b, c);
    }
    uActorStop();
    
#elif defined( TASK )

    _Task Multiply {
	const poly_t & a;
    const poly_t & b;
    poly_t & c;
    size_t startIndex, endIndex;
    const size_t delta;

    void main(){
        if(startIndex == endIndex){
            calculate(a, b, c, delta, startIndex);
        }else {
            size_t mid = (startIndex+endIndex)/2;
            if(mid > startIndex){
                Multiply right = Multiply(a, b, c, mid, endIndex, delta);
            }
            Multiply left = Multiply(a, b, c, startIndex, mid, delta);
        }
        
    }
      public:
	Multiply( const poly_t & a, const poly_t & b, poly_t & c,  
		  size_t startIndex, size_t endIndex, const size_t delta ):
          a{a}, b{b}, c{c}, startIndex{startIndex}, endIndex{endIndex}, delta{delta} {}
    };
    
    Multiply(a,b,c, 0, delta, delta);

    
#else
    #error invalid kind of concurrency system
#endif
    
} // polymultiply
