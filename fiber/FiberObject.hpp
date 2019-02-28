#pragma once

#define  BOOST_ALL_NO_LIB

#include <list>
#include <array>
#include <atomic>
#include <memory>
#include <optional>
#include <exception>
#include <shared_mutex>
#include <boost/context/fiber.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>

namespace fiber_function_basic {

    enum class FiberState {
        Yield,
        Finished,
        Error,
        Calling,
    };

    /*
        void doCall() ;
        void doOnException(std::exception_ptr) noexcept;
    */
    template<typename TParent>
    class FiberObject {
    public:
        using fiber_type = boost::context::fiber;
        using fiber_basic_type = TParent;
    private:
        class Data {
        public:
            fiber_type * runtimeFiber{ nullptr };
            FiberState state{ FiberState::Yield };
            std::optional<fiber_type> fiber;
        };
        std::shared_ptr< Data > mmmData;
    public:
        inline FiberObject(std::size_t = 1024uLL * 1024uLL * 10uLL);
    public:
        FiberObject(const FiberObject &) = default;
        FiberObject(FiberObject &&) = default;
        FiberObject&operator=(const FiberObject &) = default;
        FiberObject&operator=(FiberObject &&) = default;
    public:
        inline void operator()() const;
    public:
        inline void yield();
    public:
        inline FiberState getFiberState() const;
    private:
        inline fiber_type thisCall(fiber_type &&);
        inline FiberState callThis();
    };

    template<typename TParent>
    inline FiberObject<TParent>::FiberObject(std::size_t argSizeOfFiber) {
        mmmData = std::make_shared<Data>();
        using this_stack =
            boost::context::protected_fixedsize_stack;
        mmmData->fiber.emplace(
            std::allocator_arg,
            this_stack{ argSizeOfFiber },
            [this](fiber_type && argFiber)->fiber_type {
            mmmData->runtimeFiber = &argFiber;
            return this->thisCall(std::move(argFiber));
        });
    }

    template<typename TParent>
    inline typename FiberObject<TParent>::fiber_type FiberObject<TParent>::thisCall(
        fiber_type && argFiber) {
        try {
            static_cast<TParent *>(this)->doCall();
        } catch (...) {
            mmmData->state = FiberState::Error;
            static_cast<TParent *>(this)->doOnException(std::current_exception());
        }
        return std::move(argFiber);
    }

    template<typename TParent>
    inline FiberState FiberObject<TParent>::callThis() {
        auto & varState = mmmData->state;
        if (varState != FiberState::Yield) {
            return varState;
        }
        varState = FiberState::Calling;
        *(mmmData->fiber) = std::move(*(mmmData->fiber)).resume();
        if (varState != FiberState::Calling) {
            return varState;
        }
        return (varState = FiberState::Finished);
    }

    template<typename TParent>
    inline void FiberObject<TParent>::operator()() const {
        auto varThis =
            const_cast<FiberObject *>(this);
        varThis->callThis();
    }

    template<typename TParent>
    inline void FiberObject<TParent>::yield() {
        assert(mmmData->runtimeFiber);
        mmmData->state = FiberState::Yield;
        *(mmmData->runtimeFiber) = std::move(*(mmmData->runtimeFiber)).resume();
    }

    template<typename TParent>
    inline FiberState FiberObject<TParent>::getFiberState() const {
        return mmmData->state;
    }

}/*namespace fiber_function_basic*/

