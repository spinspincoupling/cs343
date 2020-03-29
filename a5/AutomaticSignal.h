#ifndef AUTOMATICSIGNAL_H
#define AUTOMATICSIGNAL_H
#include <vector>

#define AUTOMATIC_SIGNAL
    std::vector<uCondition*> signalledQueue; \

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        auto index = signalledQueue.size(); \
        uCondition *blocked = new uCondition(); \
        blocked->wait(); \
        signalledQueue.emplace_back(blocked); \
        signalledQueue[index]->wait(); \
        while(!(pred)){ \
            if(index+1 < signalledQueue.size()) signalledQueue[index+1]->signal(); \
            signalledQueue[index]->wait(); \
        } \
        delete signalledQueue[index]; \
        signalledQueue.erase(signalledQueue.begin()+index); \
        after; \
    } 
    
    
#define EXIT() \
    if(signalledQueue.size() > 0){ \
        signalledQueue[0]->signal(); \
    }

#endif
    