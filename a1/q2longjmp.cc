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
jmp_buf jumpE1,jumpE2,jumpE3;
vector<int> stack;

void throwException(){
    if(stack.size() > 0){
        if(stack.back() == 1){
            longjmp(jumpE1, 1);
        } else {
            longjmp(jumpE2, 1);
        }  
    } else {
        longjmp(jumpE3, 1);
    }  
}

long int Ackermann( long int m, long int n ) {
    calls += 1;
    if ( m == 0 ) {
        if ( rand() % eperiod == 0 ) { 
            PRT( T t; ) excepts += 1; 
            throwException(); 
        }
        return n + 1;
    } else if ( n == 0 ) {
            if(setjmp(jumpE1) == 0) {
                stack.emplace_back(1);
                return Ackermann( m - 1, 1 );
            } else {
                stack.pop_back();
                PRT( cout << "E1 " << m << " " << n << endl );
                if ( rand() % eperiod == 0 ) { 
                    PRT( T t; ) excepts += 1; 
                    throwException();
                }
            }
    } else {
            if(setjmp(jumpE2) == 0){
                stack.emplace_back(2);
                return Ackermann( m - 1, Ackermann( m, n - 1 ) );
            } else {
                stack.pop_back();
                PRT( cout << "E2 " << m << " " << n << endl );
                if ( rand() % eperiod == 0 ) { 
                    PRT( T t; ) excepts += 1; 
                    throwException();
                }
            }
    } // if
    return 0;                                           // recover by returning 0
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
    if (setjmp(jumpE3) == 0){                                              // begin program
        PRT( cout << m << " " << n << " " << seed << " " << eperiod << endl );
        long int val = Ackermann( m, n );
        PRT( cout << val << endl );
    } else {
        PRT( cout << "E3" << endl );
    }
    cout << "calls " << calls << ' ' << " exceptions " << excepts << endl;
}
