#ifndef AUTOMATICSIGNAL_H
#define AUTOMATICSIGNAL_H
#include <vector>

#define AUTOMATIC_SIGNAL std::vector<uCondition*> signalledQueue; \
        unsigned int current= 0

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        auto index = signalledQueue.size(); \
        uCondition *blocked = new uCondition(); \
        signalledQueue.emplace_back(blocked); \
        signalledQueue[index]->wait(); \
        while(!(pred)){ \
            ++current; \
            if(current < signalledQueue.size()) signalledQueue[current]->signal(); \
            signalledQueue[current-1]->wait(); \
        } \
        delete signalledQueue[current]; \
        signalledQueue.erase(signalledQueue.begin()+current); \
        after; \
    } 
    
    
#define EXIT() \
    if(signalledQueue.size() > 0){ \
        current = 0;\
        signalledQueue[0]->signal(); \
    }

#endif
    