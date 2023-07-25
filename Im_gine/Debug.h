#pragma once

#ifndef IMGINE_DEBUG
#define IMGINE_DEBUG
#include <boost/log/trivial.hpp>



#define LOG_DEBUG(msg) BOOST_LOG_TRIVIAL(debug) << msg;
#define LOG_INFO(msg) BOOST_LOG_TRIVIAL(info) << msg;
#define LOG_ERROR(msg) BOOST_LOG_TRIVIAL(error) << msg;
#define LOG_FATAL(msg) BOOST_LOG_TRIVIAL(fatal) << msg;

#endif