#pragma once

#define  BOOST_ALL_NO_LIB

#include <list>
#include <array>
#include <atomic>
#include <memory>
#include <cassert>
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

    template<typename TParent, typename FiberDataType = FiberData >
    class FiberObject;
    class FiberData {
    public:
        using fiber_type = boost::context::fiber;
        static inline constexpr std::size_t getDefaultStackSize();
    private:
        std::size_t const stackSize;
        fiber_type * runtimeFiber{ nullptr };
        FiberState state{ FiberState::Yield };
        std::optional<fiber_type> fiber;
        template<typename, typename>
        friend class ::fiber_function_basic::FiberObject;
    public:
        inline FiberData(std::size_t = getDefaultStackSize());
        inline virtual ~FiberData() = default;
    public:
        inline std::size_t getStackSize() const;
    public:
        FiberData(const FiberData &) = delete;
        FiberData(FiberData &&) = delete;
        FiberData&operator=(const FiberData &) = delete;
        FiberData&operator=(FiberData &&) = delete;
    };

    inline FiberData::FiberData(std::size_t argSS) :stackSize(argSS) {
    }

    inline std::size_t FiberData::getStackSize() const {
        return stackSize;
    }

    inline constexpr std::size_t FiberData::getDefaultStackSize() {
        return 1024uLL * 1024uLL * 10uLL;
    }

    /*
    TParent：
        void doCall() ;
        void doOnException(std::exception_ptr) noexcept;
    FiberDataType：
        child of FiberData
    */
    template<typename TParent, typename FiberDataType >
    class FiberObject {
        static_assert(std::is_base_of_v<FiberData, FiberDataType>);
    public:
        using fiber_type = FiberData::fiber_type;
    private:
        std::shared_ptr< FiberDataType > mmmData;
    public:
        inline FiberObject(std::shared_ptr<FiberDataType>/**/
            = std::make_shared<FiberDataType>());
    public:
        FiberObject(const FiberObject &) = default;
        FiberObject(FiberObject &&) = default;
        FiberObject&operator=(const FiberObject &) = default;
        FiberObject&operator=(FiberObject &&) = default;
    public:
        inline FiberState getFiberState() const;
    public:
        inline std::shared_ptr< FiberDataType > getFiberData() const;
    public:
        inline void _fiberCall() const;
    private:
        inline void operator()() const;
    public:
        inline void _fiberYield() const;
    private:
        inline fiber_type thisCall(fiber_type &&);
        inline FiberState callThis();
    };

    template<typename TParent, typename FiberDataType >
    inline FiberObject<TParent, FiberDataType>::FiberObject(
        std::shared_ptr<FiberDataType> argSizeOfFiber) {
        mmmData = std::move(argSizeOfFiber);
        using this_stack =
            boost::context::protected_fixedsize_stack;
        mmmData->FiberData::fiber.emplace(
            std::allocator_arg,
            this_stack{ mmmData->FiberData::stackSize },
            [this](fiber_type && argFiber)->fiber_type {
            mmmData->FiberData::runtimeFiber = &argFiber;
            return FiberObject::thisCall(std::move(argFiber));
        });
    }

    template<typename TParent, typename FiberDataType>
    inline typename FiberObject<TParent, FiberDataType>::fiber_type
        FiberObject<TParent, FiberDataType>::thisCall(
            fiber_type && argFiber) {
        try {
            static_cast<TParent *>(this)
                ->doCall();
        } catch (...) {
            mmmData->FiberData::state = FiberState::Error;
            static_cast<TParent *>(this)
                ->doOnException(std::current_exception());
        }
        return std::move(argFiber);
    }

    template<typename TParent, typename FiberDataType>
    inline FiberState FiberObject<TParent, FiberDataType>::callThis() {
        auto & varState = mmmData->FiberData::state;
        if (varState != FiberState::Yield) {
            return varState;
        }
        varState = FiberState::Calling;
        *(mmmData->FiberData::fiber) = 
            std::move(*(mmmData->FiberData::fiber)).resume();
        if (varState != FiberState::Calling) {
            return varState;
        }
        return (varState = FiberState::Finished);
    }

    template<typename TParent, typename FiberDataType>
    inline void FiberObject<TParent, FiberDataType>::operator()() const {
        auto varThis =
            const_cast<FiberObject *>(this);
        varThis->FiberObject::callThis();
    }

    template<typename TParent, typename FiberDataType>
    inline void FiberObject<TParent, FiberDataType>::_fiberCall() const {
        (*this).FiberObject::operator()();
    }

    template<typename TParent, typename FiberDataType>
    inline void FiberObject<TParent, FiberDataType>::_fiberYield() const {
        assert(mmmData->runtimeFiber);
        mmmData->FiberData::state = FiberState::Yield;
        *(mmmData->FiberData::runtimeFiber) = 
            std::move(*(mmmData->FiberData::runtimeFiber)).resume();
    }

    template<typename TParent, typename FiberDataType>
    inline FiberState FiberObject<TParent, FiberDataType>::getFiberState() const {
        return mmmData->FiberData::state;
    }

    template<typename TParent, typename FiberDataType>
    inline std::shared_ptr< FiberDataType >
        FiberObject<TParent, FiberDataType>::getFiberData() const {
        return mmmData;
    }

}/*namespace fiber_function_basic*/

#ifndef yield_the_fiber
#define yield_the_fiber(_ARG_FIBER_CALLER_) { \
    const auto & vArFBCER_0x3367 = _ARG_FIBER_CALLER_ ; \
    vArFBCER_0x3367._fiberYield() ; } \
    static_assert(true) /**/
#endif

#ifndef call_the_fiber
#define call_the_fiber(_ARG_FIBER_CALLER_) { \
    const auto & vArFBCER_0x3367 = _ARG_FIBER_CALLER_ ; \
    vArFBCER_0x3367._fiberCall() ; } \
    static_assert(true) /**/
#endif


