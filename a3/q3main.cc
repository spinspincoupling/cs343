#include "q3buffer.h"
#include <iostream>
#include <cstring>
using namespace std;

int main( int argc, char * argv[] ) {
    unsigned int cons=5 , prods=3 , produce=10 , bufferSize=10 , delays; 
    //int seed = getpid();
    try {                                               // process command-line arguments
        switch ( argc ) {
          case 6: delays = stoi( argv[5] ); if ( delays <= 0 ) throw 1;
          case 5: bufferSize = stoi( argv[4] ); if ( bufferSize <= 0 ) throw 1;
          case 4: produce = stoi( argv[3] ); if ( produce <= 0 ) throw 1;
          case 3: prods = stoi( argv[2] ); if ( prods <= 0 ) throw 1;
          case 2: cons = stoi( argv[1] ); if ( cons <= 0 ) throw 1;
                  if(argc < 6) delays = cons+prods;
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " buffer [ Cons >0 [ Prods >0 [ Produce >0 [ BufferSize >0 [ Delays >0 ] ] ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    #ifdef __U_MULTI__
    uProcessor p[3] __attribute__(( unused )); // create 3 kernel thread for a total of 4
    #endif // __U_MULTI__
    BoundedBuffer<int> buffer = BoundedBuffer<int>(bufferSize);
    int subtotal[cons];
    Producer *producers[prods];
    Consumer *consumers[cons];
    int total = 0;
    for(unsigned int i=0; i< prods; ++i){
        producers[i] = new Producer(buffer, produce, delays);
    }
    for(unsigned int i=0; i< cons; ++i){
        consumers[i] = new Consumer(buffer, delays, SENTINEL, subtotal[i] );
    }
    for(unsigned int i=0; i< prods; ++i){
        delete producers[i];
    }
    for(unsigned int i=0; i< cons; ++i){
        buffer.insert(SENTINEL);
    }
    for(unsigned int i=0; i< cons; ++i){
        delete consumers[i];
        total += subtotal[i];
    }
    cout << "total: " << total << endl;
}