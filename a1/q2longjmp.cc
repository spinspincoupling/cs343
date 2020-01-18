#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
#include <unistd.h>                                     // access: getpid
#include <setjmp.h>
#include <vector>
using namespace std;

#ifdef NOOUTPUT
#define PRT( stmt )
#else
#define PRT( stmt ) stmt
#endif // NOOUTPUT

struct E {};                                            // exception type
PRT( struct T { ~T() { cout << "~"; } }; )
long int eperiod = 100, excepts = 0, calls = 0;        // exception period
jmp_buf jumpTo;

long int Ackermann( long int m, long int n ) {
    calls += 1;
    jmp_buf jumpToPreviousStack;
    long int result;
    memcpy(jumpToPreviousStack, jumpTo, sizeof(jmp_buf));
    if ( m == 0 ) {
        if ( rand() % eperiod == 0 ) { 
            PRT( T t; ) excepts += 1; 
            //memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
            longjmp(jumpTo, 1);
        }
        return n + 1;
    } else if ( n == 0 ) {
            if(setjmp(jumpTo) == 0) {
                result = Ackermann( m - 1, 1 );
                memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
                return result;
            } else {
                PRT( cout << "E1 " << m << " " << n << endl );
                if ( rand() % eperiod == 0 ) { 
                    PRT( T t; ) excepts += 1; 
                    memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
                    longjmp(jumpTo, 1);
                }
            }
    } else {
            if(setjmp(jumpTo) == 0){
                //memcpy(jumpToPreviousStack, jumpTo, sizeof(jmp_buf));
                result = Ackermann( m, n - 1 );
                //memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
                result = Ackermann( m - 1, result);
                memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
                return result;
            } else {
                PRT( cout << "E2 " << m << " " << n << endl );
                if ( rand() % eperiod == 0 ) { 
                    PRT( T t; ) excepts += 1; 
                    memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
                    longjmp(jumpTo, 1);
                }
            }
    } // if
    memcpy(jumpTo, jumpToPreviousStack, sizeof(jmp_buf));
    return 0;                                          // recover by returning 0
}
int main( int argc, char * argv[] ) {
    long int m = 4, n = 6, seed = getpid();             // default values
    try {                                               // process command-line arguments
        switch ( argc ) {
          case 5: if ( strcmp( argv[4], "d" ) != 0 ) {  // default ?
                eperiod = stoi( argv[4] ); if ( eperiod <= 0 ) throw 1;
            } // if
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                n = stoi( argv[2] ); if ( n < 0 ) throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                m = stoi( argv[1] ); if ( m < 0 ) throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ m (>= 0) | d [ n (>= 0) | d"
             " [ seed (> 0) | d [ eperiod (> 0) | d ] ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    srand( seed );                                      // seed random number
    if (setjmp(jumpTo) == 0){                                              // begin program
        PRT( cout << m << " " << n << " " << seed << " " << eperiod << endl );
        long int val = Ackermann( m, n );
        PRT( cout << val << endl );
    } else {
        PRT( cout << "E3" << endl );
    }
    cout << "calls " << calls << ' ' << " exceptions " << excepts << endl;
}
