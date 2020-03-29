#define AUTOMATIC_SIGNAL
    #include <vector>
    struct Task{
        condexpr expr;
        uCondition &blocked;
    };
    std::Vector<Task> signalled;

#define WAITUNTIL( pred, before, after )
    if(!(pred)){
        before;
        Task t{.expr=pred};
        blocked.wait();
        after;
    }
    

#define EXIT() // select next one to go if any one blocked
    for(int i=0; i<signalled.size(); ++i){
        if(signalled[i].expr){
            signalled[i].blocked.signal();
            signaled.erase(i);
        }
    }