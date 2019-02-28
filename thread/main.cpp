#include "MainWindow.hpp"
#include <QApplication>
#include <functional>
#include <optional>
#include "CallInThisThread.hpp"
#include <iostream>

#include "TextEditTest.hpp"

class AbstractX;
class A1;
class A2;

class AbstractX {
public:
    inline int add(int a, int b);
    inline int sub(int a, int b);
    inline ~AbstractX();
protected:
    virtual int doAdd(int, int) = 0;
    virtual int doSub(int, int) = 0;
private:
    std::shared_ptr< A1 > mmmA1;
    std::shared_ptr< A2 > mmmA2;
};

inline AbstractX::~AbstractX() {
    mmmA1.reset();
    mmmA2.reset();
}

class A1 {
public:
    virtual ~A1() = default;
    virtual int doAdd(AbstractX *, decltype(&AbstractX::add), int, int) = 0;
};

class A2 {
public:
    virtual ~A2() = default;
    virtual int doSub(AbstractX*, decltype(&AbstractX::sub), int, int) = 0;
};

inline int AbstractX::add(int a, int b) {
    if (!mmmA1) {
        return this->doAdd(a, b);
    } else {
        return mmmA1->doAdd(this, &AbstractX::doAdd, a, b);
    }
}

inline int  AbstractX::sub(int a, int b) {
    if (!mmmA2) {
        return this->doSub(a, b);
    } else {
        return mmmA2->doSub(this, &AbstractX::doSub, a, b);
    }
}

class DefaultX :
    public AbstractX {
};

class Jxxx {
public:
    int foo(int, int) const {
        return 0;
    }
};

template<typename T>
class Set {
public:
    virtual bool isEqual(T *) const = 0;
    virtual void setValue(T *) const = 0;
    virtual ~Set() = default;
};

class M11 {
    int mmmVale{ 0 };
    int * pppSetValue();
    void pppValueChanged();
public:
    int getValue();
    template<typename T>
    inline M11 & setValue(T && arg);
};

int * M11::pppSetValue() {
    return &mmmVale;
}

int M11::getValue() {
    return mmmVale;
}

template<typename T, typename U>
class Set1 final : public Set<U> {
    static_assert(std::is_reference_v<T>);
public:
    T && data;
    template<typename K>
    inline Set1(K && v) :
        data(std::forward<K>(v)) {
    }
    inline bool isEqual(U * arg) const override {
        return *arg == data;
    }
    inline void setValue(U * arg) const override {
        *arg = std::forward<T&&>(data);
    }
};

template<typename T>
inline M11 & M11::setValue(T && arg) {
    if (arg == getValue()) {
        return *this;
    }
    *pppSetValue() = std::forward<T >(arg);
    pppValueChanged();
    return *this;
}

void M11::pppValueChanged() {
}

class M22 {
    int mmmValue{ 0 };
    template<typename T>
    void _setValue(T &&);
public:
    int getValue() const {
        return mmmValue;
    }
    void setValue(const int & v);
    void setValue(int &&);
};

void M22::setValue(const int & arg) {
    _setValue(arg);
}

void M22::setValue(int &&arg) {
    _setValue(std::move(arg));
}

template<typename T>
void M22::_setValue(T && arg) {
    if (mmmValue == arg) {
        return;
    }
    mmmValue = std::forward<T>(arg);
}

namespace private_namespace {
    template<typename>
    class friend_class;
}

class AAB {
    template<typename>
    friend class ::private_namespace::friend_class;

    int mmmVale{ 0 };

    template<typename ArgT>
    inline void templateSetValue(ArgT &&);

public:

    inline int getValue() const;
    inline void setValue(const int &);
    inline void setValue(int &&);
    inline void setValueDefault(int);

};

inline int AAB::getValue() const {
    return mmmVale;
}

inline void AAB::setValueDefault(int a) {
    setValue(std::move(a));
}

template<typename ArgT>
inline void AAB::templateSetValue(ArgT && arg) {
    mmmVale = std::forward<ArgT>(arg);
}

namespace private_namespace {
    template<>
    class friend_class<::AAB> {
        using type_of_this = ::AAB;
    public:
        template<typename T>
        inline static void setValue(type_of_this * arg, T && v) {
            arg->mmmVale = std::forward<T>(v);
        }
    };
}

inline void AAB::setValue(const int & a) {
    return
        ::private_namespace::friend_class<AAB>::setValue(this, a);
}
inline void AAB::setValue(int && a) {
    return
        ::private_namespace::friend_class<AAB>::setValue(this, std::move(a));
}

#include "CopyDirHelper.hpp"


class X {
public:
    int arg;
    int ans;

    X(int a) :arg(a) {
    }

};

//f(n) = f(n-1) + f(n-2)

#define  BOOST_ALL_NO_LIB
#include <boost/context/fiber.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>
//"D:\Qt\5.12.1\msvc2017_64\lib\boost_context-vc141-mt-gd-x64-1_68.lib"
#pragma comment(lib,"D:/Qt/5.12.1/msvc2017_64/lib/boost_context-vc141-mt-gd-x64-1_68.lib")

namespace ctx = boost::context;

#include <set>
#include <map>
std::map<int, int>testA;

class X111 {
public:
    X * value;
public:

    X111(X * arg) : value{ arg } {
    }

    ctx::fiber operator()(ctx::fiber && f) {

        std::cout << value->arg << std::endl;

        if (value->arg < 2) {
            value->ans = 1;
            return std::move(f);
        }

        {
            X var1{ value->arg - 1 };

            {
                ctx::fiber f1{
                    std::allocator_arg_t{},
                    boost::context::protected_fixedsize_stack{ 1024 * 1024 * 10 },
                    X111{ &var1 } };
                f1 = std::move(f1).resume();
            }

            X var2{ value->arg - 2 };
            {
                ctx::fiber f2{
                    std::allocator_arg_t{},
                    boost::context::protected_fixedsize_stack{ 1024 * 1024 * 10 },
                    X111{ &var2 } };
                f2 = std::move(f2).resume();
            }

            value->ans = var1.ans + var2.ans;
        }

        // testA[value->arg] = value->ans ;

        return std::move(f);
    }

    static inline int eval(int v) {

        X var1{ v };
        ctx::fiber f1{ X111{ &var1 } };
        f1 = std::move(f1).resume();
        return var1.ans;

    }

};


int fbbb(int a) {
    if (a < 2) {
        return 1;
    }
    return fbbb(a - 1) + fbbb(a - 2);
}

#include <iostream>

inline constexpr std::int64_t operator""_i64(unsigned long long v) noexcept {
    return static_cast<std::int64_t>(v);
}

inline constexpr std::uint64_t operator""_u64(unsigned long long v) noexcept {
    return static_cast<std::uint64_t>(v);
}



template<typename ... Args>
void jooo(Args && ...) {
}


template<typename ... Args>
void foo(Args && ... args) {

    auto j = [args = std::forward<Args>(args) ...]() {
        jooo(args  ...);
    };

    j();

}

#include "ThreadDuty.hpp"
#include "ThreadManager.hpp"

class Test112 : public ThreadDuty {
public:
    void doCall() override {

        std::cout << std::endl << "?1" << std::endl;
        yield();
        std::cout << std::endl << "?2" << std::endl;
    }
};



void test1() {


    auto varTest = new  Test112;
    auto varManager = getThreadManager();
    varManager->call(varTest);


}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    test1();

    foo(1, 2, 3, 4);

    std::cout << 12_i64 << std::endl;
    std::cout << 12_u64 << std::endl;
    std::cout << -12_i64 << std::endl;

    if constexpr (false) {
        int ans = 0;
        ctx::fiber f1{
            std::allocator_arg_t{},
            boost::context::protected_fixedsize_stack{
            1024ull *
            1024ull *
            1024ull *
            10ull   },
            [&ans](boost::context::fiber && f)->boost::context::fiber {
            ans = fbbb(1024 * 1024 * 10);
            return std::move(f);
        } };
        f1 = std::move(f1).resume();
    }

    //std::cout <<
    //    fbbb( 1024 * 100   )
    //    << std::endl;

    //std::cout <<
    //    X111::eval(1024 * 100)
    //    << std::endl;


    M11 m1;
    m1.setValue(12);

    sizeof(int) + sizeof(long);

    TextEdit test;
    test.show();

    {
        copy_dir_helper::CopyDirHelper c{ "F:/test1","F:/test2" };

        const auto & varX =
            c.getCopyDirs();
        const auto & varY =
            c.getCopyFiles();
    }

    {
        auto var =
            copy_dir_helper::make("F:/test1", "F:/test2");
        const auto & varX =
            var->getCopyDirs();
        const auto & varY =
            var->getCopyFiles();
    }

    Jxxx x;
    std::invoke(&Jxxx::foo, &x, 1, 2);

    qDebug() <<
        test.document()->documentLayout();

    //MainWindow w;
    //w.show();

    return a.exec();
}


#include <shared_mutex>


void xxxx(const void * arg) {

    //    static_assert<std::uptr >();
    auto varPointer =
        reinterpret_cast<std::uintptr_t>(arg);

    ((varPointer >> 2)&(127u));

}



void koo(std::shared_ptr<void>, int arg) {

    switch (arg) {
    case 0:goto label_0;
    case 1:goto label_1;
    case 2:goto label_2;
    case 3:goto label_3;
    }

label_0:
    {
    }

label_1:
    {
    }

label_2:
    {
    }

label_3:
    {
    }

}











