#pragma once

#include "ThreadGrobal.hpp"

class ThreadDuty;
void intrusive_ptr_add_ref(const ThreadDuty *);
void intrusive_ptr_release(const ThreadDuty *);

class ThreadDuty {
public:
    enum class State {
        Yield,
        Finished,
        Error,
        Calling,
    };
private:
    mutable std::atomic_int_fast64_t mmmUserCount{ 0 };
    using Fiber = boost::context::fiber;
    std::optional< Fiber > mmmFiber;
    Fiber * mmmYieldFiber{ nullptr };
    State mmmState{ State::Yield };
public:
    ThreadDuty();
    virtual ~ThreadDuty();
private:
    State call();
protected:
    void yield();
    void yield(int ms);
protected:
    virtual void doCall() = 0;
    virtual void doOnException(std::exception_ptr) noexcept;
private:
    Fiber thisCall(Fiber &&);
public:
    ThreadDuty(ThreadDuty &&) = delete;
    ThreadDuty(const ThreadDuty &) = delete;
    ThreadDuty&operator=(ThreadDuty &&) = delete;
    ThreadDuty&operator=(const ThreadDuty &) = delete;
public:
    friend void intrusive_ptr_add_ref(const ThreadDuty *);
    friend void intrusive_ptr_release(const ThreadDuty *);
    friend class ThreadCaller;
    friend class ThreadManager;
};


