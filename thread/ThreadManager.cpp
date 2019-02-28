#include "ThreadManager.hpp"

class ThreadCaller {
    Thread * mmmThisThread;
    boost::intrusive_ptr< ThreadDuty > mmmCurrentDuty;
public:
    inline ThreadCaller(
        Thread * argThread,
        boost::intrusive_ptr< ThreadDuty > arg) :
        mmmThisThread(argThread),
        mmmCurrentDuty(std::move(arg)) {
    }

    inline void operator()() {
        mmmCurrentDuty->call();
        giveThreadBack();
    }

    inline void giveThreadBack() {
        auto * varManager = getThreadManager();
        varManager->giveBackThread(mmmThisThread);
    }

};

class WakeUpDuty :
    public ThreadDuty {
protected:
    void doCall() override {
    }
};

ThreadManager::ThreadManager() {
    static boost::intrusive_ptr varWakeUpDuty
        = new WakeUpDuty;
    mmmWakeUpDuty = varWakeUpDuty;
    for (int i = 0; i < 12; ++i) {
        mmmWaitToCall.push_back(new Thread);
    }
}

void ThreadManager::wakeUpAllThread() {
    std::size_t varSize = 0;
    {
        std::shared_lock varReadLock{ mmmCallThreadMutex };
        varSize = mmmWaitToCall.size();
    }
    for (std::size_t i = 0; i < varSize; ++i) {
        this->call(mmmWakeUpDuty);
    }
}

void ThreadManager::giveBackThread(Thread * arg) {

    do {
        {
            std::shared_lock varReadLock{ mmmDutysMutex };
            if (!mmmDutys.empty()) {
                break;
            }
        }
        {
            std::unique_lock varWriteLock{ mmmCallThreadMutex };
            mmmWaitToCall.splice(mmmWaitToCall.begin(),
                mmmCalling,
                arg->getPosInThreadManager());
            return;
        }
    } while (false);

    {
        boost::intrusive_ptr< ThreadDuty > varDuty;
        {
            std::unique_lock varWriteLock{ mmmDutysMutex };
            if (mmmDutys.empty()) {
                std::unique_lock varWriteLock{ mmmCallThreadMutex };
                mmmWaitToCall.splice(mmmWaitToCall.begin(),
                    mmmCalling,
                    arg->getPosInThreadManager());
                return;
            }
            varDuty = std::move(mmmDutys.front());
            mmmDutys.pop_front();
        }
        this->call(arg, std::move(varDuty));
        this->wakeUpAllThread();
        return;
    }

}

void ThreadManager::call(
    Thread * argThread,
    boost::intrusive_ptr< ThreadDuty > && argDuty) {
    QMetaObject::invokeMethod(
        argThread->getThreadObject(),
        ThreadCaller{ argThread , std::move(argDuty) },
        Qt::QueuedConnection);
}

template< ThreadManager::InsertType  N >
inline void ThreadManager::callDetail(boost::intrusive_ptr< ThreadDuty > && arg) {

    do {
        /*如果有空闲线程则执行...*/
        ThreadList::iterator varPos;
        {
            std::unique_lock mmmWriteLock{ mmmCallThreadMutex };
            if (!mmmWaitToCall.empty()) {
                mmmCalling.splice(
                    mmmCalling.begin(),
                    mmmWaitToCall,
                    mmmWaitToCall.begin());
                varPos = mmmCalling.begin();
            } else {
                break;
            }
        }

        Thread * varThread = *varPos;
        varThread->setPosInThreadManager(varPos);
        if (arg == mmmWakeUpDuty) {
            /*如果是唤醒事件则尝试从队列中获取事件*/
            std::unique_lock varWriteLock{ mmmDutysMutex };
            if (!mmmDutys.empty()) {
                arg = std::move(mmmDutys.front());
                mmmDutys.pop_front();
            }
        }
        this->call(varThread, std::move(arg));

        return;
    } while (false);

    /*所有线程已经唤醒放弃WakeUp事件*/
    if (arg == mmmWakeUpDuty) {
        return;
    }

    /*加入等待执行列表*/
    std::unique_lock varWriteLock{ mmmDutysMutex };
    if constexpr (N == InsertType::front) {
        mmmDutys.push_front(std::move(arg));
    } else {
        mmmDutys.push_back(std::move(arg));
    }

}

void ThreadManager::call(boost::intrusive_ptr< ThreadDuty > arg) {
    callDetail<InsertType::back>(std::move(arg));
}

void ThreadManager::yieldDuty(
    boost::intrusive_ptr<ThreadDuty> arg,
    int argMs) {
    auto varCurrentThread =
        static_cast<Thread *>(QThread::currentThread());
    if (argMs < 1) {
        QMetaObject::invokeMethod(
            varCurrentThread->getThreadObject(),
            [this, var = std::move(arg)]() mutable {
            this->callDetail<InsertType::back>(std::move(var));
        }, Qt::QueuedConnection);
    } else {
        QTimer::singleShot(
            argMs,
            varCurrentThread->getThreadObject(),
            [this, var = std::move(arg)]() mutable {
            this->callDetail<InsertType::front>(std::move(var));
        });
    }
}

ThreadManager::~ThreadManager() {
    {
        std::unique_lock mmmWriteLock{ mmmCallThreadMutex };
        for (auto varI : mmmCalling) {
            varI->quit();
        }
        for (auto varI : mmmWaitToCall) {
            varI->quit();
        }
        mmmCalling.clear();
        mmmWaitToCall.clear();
    }
}

ThreadManager * getThreadManager() {
    static auto varAns = new ThreadManager;
    return varAns;
}







