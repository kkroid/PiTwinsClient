#pragma once

#include "platform_config.h"

#ifdef __cplusplus
#define GOOGLE_GLOG_DLL_DECL           // 使用静态glog库时，必须定义这个
#define GLOG_NO_ABBREVIATED_SEVERITIES // 没这个编译会出错,传说因为和Windows.h冲突

#define LOG_TRACE std::cout << __FILE__ << ":" << __LINE__ << " "
#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << " "
#define LOG_INFO  std::cout << __FILE__ << ":" << __LINE__ << " "
#define LOG_WARN  std::cout << __FILE__ << ":" << __LINE__ << " "
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << " "
#define LOG_FATAL std::cout << __FILE__ << ":" << __LINE__ << " "
#define DLOG_TRACE std::cout << __PRETTY_FUNCTION__ << " this=" << this << " "
#define DLOG_WARN std::cout << __PRETTY_FUNCTION__ << " this=" << this << " "
#define CHECK_NOTnullptr(val) LOG_ERROR << "'" #val "' Must be non nullptr";
#endif // end of define __cplusplus

//#ifdef _DEBUG
//#ifdef assert
//#undef assert
//#endif
//#define assert(expr)  { if (!(expr)) { LOG_FATAL << #expr ;} }
//#endif