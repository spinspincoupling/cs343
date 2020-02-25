#include "q2polymult.h"
#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta ) {
    const int size = sizeof(c)/sizeof(*c);
    const int sizep = sizeof(a)/sizeof(*a);

#if defined( CFOR )
    COFOR(b, 0, delta,
        int index=b; 
        while(index < size){
            int total = 0;
            for(int i=0; i<=index; ++i){
                if(i+sizep-1 < index) continue;
                total += a[i]*b[index-i];
            }
            c[index] = total;
            index += delta;
        }
	);
    
#elif defined( ACTOR )

    struct WorkMsg : public uActor::Message {
	int delta;
    int start;
    const poly_t & a;
    const poly_t & b;
    poly_t & c;
	WorkMsg(int delta, int start, const poly_t & a, const poly_t & b, poly_t & c) : 
    Message( uActor::Delete ), delta{delta}, start{start}, a{a}, b{b}, c{c} {}
    };

    _Actor Multiply {
	Allocation receive( Message & w ) {
	    Case( WorkMsg, w ) {			// discriminate derived message
            int index = w_d->start;
            int delta = w_d->delta;
            const int size = sizeof(c)/sizeof(*c);
            const int sizep = sizeof(a)/sizeof(*a);
            const poly_t & a = w_d->a;
            const poly_t & b = w_d->b;
            while(index < size){
            int total = 0;
                for(int i=0; i<=index; ++i){
                    if(i+sizep-1 < index) continue;
                    total += a[i]*b[index-i];
                }
            (w_d->c)[index] = total;
            index += delta;
            }
	    } else assert( false );			// bad message
	    return Finished;				// one-shot
	} // Multiply::receive
    }; // Multiply

    uActorStart();					// start actor system
    for(int i=0; i<delta; ++i){
        Multiply() | new WorkMsg(delta, i);
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
            const int size = sizeof(c)/sizeof(*c);
            const int sizep = sizeof(a)/sizeof(*a);
            int index=startIndex; 
            while(index < size){
            int total = 0;
                for(int i=0; i<=index; ++i){
                    if(i+sizep-1 < index) continue;
                    total += a[i]*b[index-i];
                }
            c[index] = total;
            index += delta;
            }
        }else {
            int mid = (startIndex+endIndex)/2;
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
