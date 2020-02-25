#include "q2polymult.h"
#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta ) {

#if defined( CFOR )
    COFOR(s, 0, (int)delta,
        int index=s; 
        while(index < c.size){
            int total = 0;
            for(int i=0; i<a.size; ++i){
                if(i+a.size-1 < index) continue;
                if(i > index) break;
                total += a.arr[i]*b.arr[index-i];
            }
            c.arr[index] = total;
            index += delta;
        }
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
            size_t index = w_d->start;
            const poly_t & a = w_d->a;
            const poly_t & b = w_d->b;
            const size_t size = (w_d->c).size;
            const size_t sizep = a.size;
            while(index < size){
            int total = 0;
                for(unsigned int i=0; i<a.size; ++i){
                    if(i+sizep-1 < index) continue;
                    if(i > index) break;
                    total += a.arr[i]*b.arr[index-i];
                }
            (w_d->c).arr[index] = total;
            index += w_d->delta;
            }
	    } else assert( false );			// bad message
	    return Finished;				// one-shot
	} // Multiply::receive
    }; // Multiply

    uActorStart();					// start actor system
    for(unsigned int i=0; i<delta; ++i){
        Multiply m = Multiply();
        m | new WorkMsg(delta, i, a, b, c);
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
            size_t index=startIndex; 
            while(index < c.size){
            int total = 0;
                for(unsigned int i=0; i<=a.size; ++i){
                    if(i+a.size-1 < index) continue;
                    if(i > index) break;
                    total += a.arr[i]*b.arr[index-i];
                }
            c.arr[index] = total;
            index += delta;
            }
        }else {
            size_t mid = (startIndex+endIndex)/2;
            if(mid > startIndex) Multiply(a, b, c, mid, endIndex, delta);
            Multiply(a, b, c, startIndex, mid, delta);
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
