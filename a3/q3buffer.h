#include <queue>
template<typename T> class BoundedBuffer {
    const unsigned int size;
    unsigned int buff;
    bool pwait;
    bool cwait;
    queue<T> queue;
    uOwnerLock mutex;
    uCondLock plock;
    uCondLock clock;
  public:
    BoundedBuffer( const unsigned int size = 10 ): size{size}, buff{0}, pwait{false}, cwait{false} {}
    void insert( T elem );
    T remove();
};

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
