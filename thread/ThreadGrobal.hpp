#pragma once

#define  BOOST_ALL_NO_LIB
#include <memory>
#include <optional>
#include <atomic>
#include <array>
#include <list>
#include <exception>
#include <shared_mutex>
#include <QtCore/QtCore>
#include <boost/context/fiber.hpp>
#include <boost/intrusive_ptr.hpp>

class ThreadManager;
class Thread;
class ThreadCaller;

using ThreadList = std::list< Thread * >;

ThreadManager * getThreadManager();









