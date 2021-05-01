//
// Created by Will Zhang on 2021/1/15.
//

#ifndef PITWINSCLIENT_UTILS_H
#define PITWINSCLIENT_UTILS_H

#include <memory>
#include <string>
#include <stdexcept>

template<typename ... Args>
std::string format( const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

#endif //PITWINSCLIENT_UTILS_H
