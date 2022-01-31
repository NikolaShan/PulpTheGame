//
// Created by Mike on 28.03.14.
//



#ifndef __SchedulerHelper_H_
#define __SchedulerHelper_H_

#include <functional>

class SchedulerHelper {
public:
    static SchedulerHelper &getInstance();
    ~SchedulerHelper() {}
    
    void schedule(std::function<void (void)> func, float delay);
    
private:
    class RandomTarget;
};


#endif //__SchedulerHelper_H_
