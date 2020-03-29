#define AUTOMATIC_SIGNAL
    #include <vector>
    std::vector<uCondition> signalled;

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        auto index = signalled.size(); \
        uCondition blocked; \
        blocked.wait(); \
        signalled.emplace_back(std::move(blocked)); \
        while(!(pred)){ \
            if(index+1 < signalled.size()) signalled[index+1].signal(); \
            signalled[index].wait(); \
        } \
        signalled.erase(signalled.begin()+index); \
        after; \
    } \
    
    
#define EXIT() \
    if(signalled.size() > 0){ \
        signalled[0].signal(); \
    }
    