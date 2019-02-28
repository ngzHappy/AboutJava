#pragma once

#include "ThreadGrobal.hpp"
#include "ThreadDuty.hpp"
#include "Thread.hpp" 

class ThreadManager {
    boost::intrusive_ptr<ThreadDuty> mmmWakeUpDuty;
    std::shared_mutex mmmCallThreadMutex;
    ThreadList mmmCalling;
    ThreadList mmmWaitToCall;
    std::shared_mutex mmmDutysMutex;
    std::list< boost::intrusive_ptr< ThreadDuty > > mmmDutys;
public:

    ThreadManager();
    virtual ~ThreadManager();

public:
    void call(boost::intrusive_ptr< ThreadDuty >);
private:
    void call(Thread *, boost::intrusive_ptr< ThreadDuty > && );
    enum class InsertType {
        back,
        front,
    };
    template< InsertType = InsertType::back >
    inline void callDetail(boost::intrusive_ptr< ThreadDuty > &&);
    void giveBackThread(Thread *);
    void yieldDuty(boost::intrusive_ptr<ThreadDuty>,int);
    void wakeUpAllThread();
    friend class ThreadCaller;
    friend class ThreadDuty;
};















