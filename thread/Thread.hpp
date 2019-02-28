#pragma once

#include "ThreadGrobal.hpp"

class Thread : public QThread {
    std::atomic< QObject * > mmmObject{ nullptr };
    ThreadList::iterator mmmPos;
public:
    Thread( );
    virtual ~Thread();
public:
    QObject * getThreadObject() const;
    ThreadList::iterator getPosInThreadManager() const;
    void setPosInThreadManager(ThreadList::iterator);
protected:
     void run() override;
};

