#ifndef AUTOMATICSIGNAL_H
#define AUTOMATICSIGNAL_H

#define AUTOMATIC_SIGNAL \
        uCondition blocked

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        blocked.wait(); \
        while(!(pred)){ \
            if(!blocked.empty()) blocked.signal(); \
            blocked.wait(); \
        } \
        after; \
    } 
    
    
#define EXIT() \
    if(!blocked.empty()) blocked.signal(); \

#endif
    