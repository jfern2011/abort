/**
 *  \file   abort.h
 *  \author Jason Fernandez
 *  \date   12/28/2019
 *
 *  https://github.com/jfern2011/abort
 */

#ifndef ABORT_H_
#define ABORT_H_

#include <cstddef>
#include <cstdio>
#include <memory>
#include <ostream>
#include <string>

#ifndef DOXYGEN_SKIP
#if defined(_WIN32) || defined(_WIN64)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif  // DOXYGEN_SKIP

namespace diagnostics {
namespace internal {

extern std::string buffer;
extern int         frame_cnt;

/**
 * Get the number of arguments passed to a variadic macro
 *
 * @tparam T The macro argument types
 *
 * @param[in] args The macro input arguments
 *
 * @return The number of arguments
 */
template <typename... T>
int get_abort_nargs(T&& ... args) { return sizeof...(T); }

void errno_msg(const char* select, const char* expr, const char* ret,
             int frame_cnt, const char* file, int line, const char* func);

void print_msg(const char* select, int num_args, const char* cond,
               const char* ret, int frame_cnt, const char* file, int line,
               const char* func, const std::string& msg);

}  // namespace internal

std::ostream& get_ostream();
void          set_message_size(std::size_t size);
void          set_ostream(std::shared_ptr<std::ostream> os);

}  // namespace diagnostics

/**
 * @def ABORT_N_ARGS(...)
 *
 * Get the number of arguments passed to a variadic macro
 */
#define ABORT_N_ARGS(...) diagnostics::internal::get_abort_nargs(__VA_ARGS__)

/**
 * @def ABORT_IF(cond, ret, ...)
 *
 * Triggers an abort in the event that the specified condition \a cond
 * is true. This will cause the currently executing function to exit
 * with the return value \a ret. Any additional arguments will be used
 * to construct an error message
 */
#define ABORT_IF(cond, ret, ...) {                                            \
    diagnostics::internal::frame_cnt++;                                       \
    if (cond) {                                                               \
        std::string message = diagnostics::internal::buffer;                  \
        if (!message.empty()) {                                               \
            std::snprintf( &message.at(0), message.size(), " " __VA_ARGS__);  \
        }                                                                     \
        diagnostics::internal::print_msg("ABORT_IF",                          \
                                         ABORT_N_ARGS(__VA_ARGS__),           \
                                         #cond, #ret,                         \
                                         diagnostics::internal::frame_cnt-1,  \
                                         __FILE__,                            \
                                         __LINE__,                            \
                                         __PRETTY_FUNCTION__,                 \
                                         message);                            \
        diagnostics::internal::frame_cnt--;                                   \
        return (ret);                                                         \
    } else {                                                                  \
        diagnostics::internal::frame_cnt--;                                   \
    }                                                                         \
}

 /**
  * @def ABORT(ret, ...)
  *
  * Triggers an unconditional abort.  This will cause the currently
  * executing function to exit with the return value \a ret. Any
  * additional arguments (optional) will be used to create an error
  * message
  */
#define ABORT(ret, ...) {                                                 \
    diagnostics::internal::frame_cnt++;                                   \
    std::string message = diagnostics::internal::buffer;                  \
    if (!message.empty()) {                                               \
        std::snprintf( &message.at(0), message.size(), " " __VA_ARGS__);  \
    }                                                                     \
    diagnostics::internal::print_msg("ABORT",                             \
                                     ABORT_N_ARGS(__VA_ARGS__),           \
                                     "", #ret,                            \
                                     diagnostics::internal::frame_cnt-1,  \
                                     __FILE__,                            \
                                     __LINE__,                            \
                                     __PRETTY_FUNCTION__,                 \
                                     message);                            \
    diagnostics::internal::frame_cnt--;                                   \
    return (ret);                                                         \
}

 /**
  * @def ABORT_IF_NOT(cond, ret, ...)
  *
  * Triggers an abort in the event that the specified condition \a cond
  * is false. This will cause the currently executing function to exit
  * with the return value \a ret. Any additional arguments will be used
  * to construct an error message
  */
#define ABORT_IF_NOT(cond, ret, ...) {                                        \
    diagnostics::internal::frame_cnt++;                                       \
    if (!(cond)) {                                                            \
        std::string message = diagnostics::internal::buffer;                  \
        if (!message.empty()) {                                               \
            std::snprintf( &message.at(0), message.size(), " " __VA_ARGS__);  \
        }                                                                     \
        diagnostics::internal::print_msg("ABORT_IF_NOT",                      \
                                         ABORT_N_ARGS(__VA_ARGS__),           \
                                         #cond, #ret,                         \
                                         diagnostics::internal::frame_cnt-1,  \
                                         __FILE__,                            \
                                         __LINE__,                            \
                                         __PRETTY_FUNCTION__,                 \
                                         message);                            \
        diagnostics::internal::frame_cnt--;                                   \
        return (ret);                                                         \
    } else {                                                                  \
        diagnostics::internal::frame_cnt--;                                   \
    }                                                                         \
}

/**
  * @def ABORT_ON_ERRNO(expr, ret)
  *
  * Triggers an abort in the event that \a expr (usually a system call)
  * returns -1. This will cause the currently executing function to
  * exit with the return value \a ret. An error message is constructed
  * based on the value of errno
  */
#define ABORT_ON_ERRNO(expr, ret) {                                           \
    diagnostics::internal::frame_cnt++;                                       \
    if ((expr) == -1) {                                                       \
        diagnostics::internal::errno_msg("ABORT_ON_ERRNO",                    \
                                         #expr, #ret,                         \
                                         diagnostics::internal::frame_cnt-1,  \
                                         __FILE__,                            \
                                         __LINE__,                            \
                                         __PRETTY_FUNCTION__);                \
        diagnostics::internal::frame_cnt--;                                   \
        return (ret);                                                         \
    } else {                                                                  \
        diagnostics::internal::frame_cnt--;                                   \
    }                                                                         \
}

#endif  // ABORT_H_
