/**
 *  \file   abort.h
 *  \author Jason Fernandez
 *  \date   10/18/2017
 *
 *  https://github.com/jfern2011/abort
 */

#ifndef __ABORT_H__
#define __ABORT_H__

#include <cerrno>
#include <cstdio>

#ifndef DOXYGEN_SKIP

/*
 * Helper function to determine number of arguments passed
 */
template <typename... T>
int _get_abort_nargs_(T&&... args) { return sizeof...(T); }

#endif

/**
 * @def AbortIf(cond, ret, ...)
 *
 * Triggers an abort in the event that the specified condition \a cond
 * is true. This will cause the currently executing function to exit
 * with the return value \a ret. Any additional arguments will be used
 * to construct an error message
 */
#define AbortIf(cond, ret, ...)                         \
{                                                       \
    if (cond)                                           \
    {                                                   \
        char errMsg[256];                               \
                                                        \
        std::snprintf(                                  \
            errMsg, 256, "[abort] %s in %s:%d",         \
            __PRETTY_FUNCTION__, __FILE__, __LINE__);   \
                                                        \
        if (errno)                                      \
        {                                               \
            std::perror(errMsg); std::fflush(stderr);   \
            errno = 0;                                  \
        }                                               \
        else                                            \
        {                                               \
            std::printf("%s", errMsg);                  \
            if (_get_abort_nargs_(__VA_ARGS__))         \
                std::printf( ": " __VA_ARGS__);         \
            std::printf("\n");                          \
                std::fflush( stdout );                  \
        }                                               \
        return (ret);                                   \
    }                                                   \
}

/**
 * @def Abort(ret, ...)
 *
 * Triggers an unconditional abort.  This will cause the currently
 * executing function to exit with the return value \a ret. Any
 * additional arguments (optional) will be used to create an error
 * message
 */
#define Abort(ret, ...) AbortIf(true, ret, ##__VA_ARGS__)

/**
 * @def AbortIfNot(cond, ret, ...)
 *
 * Triggers an abort in the event that the specified condition \a cond
 * is false. This will cause the currently executing function to exit
 * with the return value \a ret. Any additional arguments will be used
 * to construct an error message
 */
#define AbortIfNot(cond, ret, ...)                      \
    AbortIf(!(cond), ret, ##__VA_ARGS__)

#endif
