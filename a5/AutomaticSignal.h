#ifndef AUTOMATICSIGNAL_H
#define AUTOMATICSIGNAL_H
#define AUTOMATIC_SIGNAL
    #include <vector>
    std::vector<uCondition*> signalled;

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        auto index = signalled.size(); \
        uCondition *blocked = new uCondition(); \
        blocked->wait(); \
        signalled.emplace_back(blocked); \
        signalled[index]->wait(); \
        while(!(pred)){ \
            if(index+1 < signalled.size()) signalled[index+1]->signal(); \
            signalled[index]->wait(); \
        } \
        delete signalled[index]; \
        signalled.erase(signalled.begin()+index); \
        after; \
    } \
    
    
#define EXIT() \
    if(signalled.size() > 0){ \
        signalled[0]->signal(); \
    }

#endif
    