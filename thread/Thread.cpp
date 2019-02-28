#include "Thread.hpp"
#include <QtCore/qcoreapplication.h>

Thread::Thread( )  {
    this->moveToThread( qApp->thread() );
    connect( this,&QThread::finished,this,&QObject::deleteLater );
    this->start();
}

Thread::~Thread() {
}

void Thread::run() {
    QObject varThisThreadObject;
    mmmObject.store(&varThisThreadObject);
    this->exec();
}

QObject * Thread::getThreadObject() const {
    return mmmObject.load();
}

ThreadList::iterator Thread::getPosInThreadManager() const {
    return mmmPos;
}

void Thread::setPosInThreadManager(ThreadList::iterator arg) {
    mmmPos = std::move(arg);
}





