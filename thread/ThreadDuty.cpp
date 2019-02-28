#include <iostream>
#include "ThreadDuty.hpp"
#include "ThreadManager.hpp"

ThreadDuty::ThreadDuty() {
    mmmFiber.emplace(
        [this](Fiber && argFiber)->Fiber {
        mmmYieldFiber = &argFiber;
        return thisCall(std::move(argFiber));
    });
}

ThreadDuty::~ThreadDuty() {
}

void intrusive_ptr_add_ref(const ThreadDuty * arg) {
    ++(arg->mmmUserCount);
}

void intrusive_ptr_release(const ThreadDuty * arg) {
    if (--(arg->mmmUserCount) < 1) {
        delete arg;
    }
}

ThreadDuty::State ThreadDuty::call() {
    if (mmmState != State::Yield) {
        return mmmState;
    }
    mmmState = State::Calling;
    *mmmFiber = std::move(*mmmFiber).resume();
    if (mmmState != State::Calling) {
        return mmmState;
    }
    return (mmmState = State::Finished);
}

void ThreadDuty::doOnException(std::exception_ptr) noexcept {
    std::cout << __func__ << "exception ignored!" << std::endl;
}

ThreadDuty::Fiber ThreadDuty::thisCall(Fiber && argFiber) {
    try {
        doCall();
    } catch (...) {
        mmmState = State::Error;
        doOnException(std::current_exception());
    }
    return std::move(argFiber);
}

void ThreadDuty::yield() {
    yield(0);
}

void ThreadDuty::yield(int ms) {
    assert(mmmYieldFiber);
    mmmState = State::Yield;
    {
        auto varManager = getThreadManager();
        varManager->yieldDuty(this,ms);
    }
    *mmmYieldFiber = std::move(*mmmYieldFiber).resume();
}





