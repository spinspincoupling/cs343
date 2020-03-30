#ifndef AUTOMATICSIGNAL_H
#define AUTOMATICSIGNAL_H

#define AUTOMATIC_SIGNAL \
        uCondition blocked; \
        unsigned int size = 0; \
        unsigned int count

#define WAITUNTIL( pred, before, after ) \
    if(!(pred)){ \
        before; \
        ++size; \
        blocked.wait(); \
        while(!(pred)){ \
            --count; \
            if(count > 0 && !blocked.empty()) blocked.signal(); \ 
            blocked.wait(); \
        } \
        --size; \
        after; \
    } 
    
    
#define EXIT() \
    count = size; \
    if(!blocked.empty()) blocked.signal(); \

#endif
    