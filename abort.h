/**
 *  \file   abort.h
 *  \author Jason Fernandez
 *  \date   10/18/2017
 *
 *  https://github.com/jfern2011/abort
 */

#ifndef ABORT_H_
#define ABORT_H_

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#ifndef DOXYGEN_SKIP
#if defined(_WIN32) || defined(_WIN64)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif  // DOXYGEN_SKIP

namespace diagnostics {

std::ostream& get_ostream(); // forward declaration

namespace internal {

/**
 * The stream to which messages are written. All messages are by default
 * written to standard output
 */
std::shared_ptr<std::ostream> stream;

/**
 * The buffer to which messages are formatted before being placed on the
 * output stream
 */
std::string buffer(1024, '\0');

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

/**
 * Write the body of a message to the stream
 *
 * @param[in] msg The message to write
 */
inline void body(const std::string& msg) {
    get_ostream() << msg << std::endl;
}

/**
 * Write the "preface" of a message
 *
 * @param[in] file The name of the file from which this message originated
 * @param[in] line The line number at which this message originated
 * @param[in] func Function (or method) from which this message originated
 */
inline void preface(const char* file, int line, const char* func) {
    
    get_ostream() << file << ":" << line << ": In '" << func << "': ";
}

}  // namespace internal

/**
 * Get the stream object currently being written to
 *
 * @return The output stream
 */
inline std::ostream& get_ostream() {
    if (!internal::stream) {
        internal::stream = std::make_shared<std::ostream>(
            std::cout.rdbuf());
    }
    return *internal::stream;
}

/**
 * Set the maximum size of an output message in bytes. Messages larger
 * than this will be truncated
 *
 * @param[in] size The message size limit
 */
inline void set_message_size(std::size_t size) {
    internal::buffer.resize(size);
}

/**
 * Set the stream object to write to. By default, messages are written
 * to standard output
 *
 * @return The output stream
 */
inline void set_ostream(std::shared_ptr<std::ostream> os) {
    internal::stream = os;
}

}  // namespace diagnostics

/**
 * @def ABORT_SELECT(select, cond, ret, ...)
 *
 * Generalized abort macro
 */
#define ABORT_SELECT(select, cond, ret, ...) {                          \
    if (cond) {                                                         \
        diagnostics::internal::preface(                                 \
            __FILE__, __LINE__, __PRETTY_FUNCTION__);                   \
        if (diagnostics::internal::get_abort_nargs(__VA_ARGS__)) {      \
            std::snprintf(&diagnostics::internal::buffer.at(0),         \
                          diagnostics::internal::buffer.size(),         \
                          select "\n\tnote: " __VA_ARGS__);             \
            diagnostics::internal::body(diagnostics::internal::buffer); \
        } else {                                                        \
            diagnostics::internal::body(select);                        \
        }                                                               \
        return (ret);                                                   \
    }                                                                   \
}

/**
 * @def ABORT_IF(cond, ret, ...)
 *
 * Triggers an abort in the event that the specified condition \a cond
 * is true. This will cause the currently executing function to exit
 * with the return value \a ret. Any additional arguments will be used
 * to construct an error message
 */
#define ABORT_IF(cond, ret, ...) \
    ABORT_SELECT("ABORT_IF("#cond", "#ret");", (cond), (ret), __VA_ARGS__)

 /**
  * @def ABORT(ret, ...)
  *
  * Triggers an unconditional abort.  This will cause the currently
  * executing function to exit with the return value \a ret. Any
  * additional arguments (optional) will be used to create an error
  * message
  */
#define ABORT(ret, ...) \
    ABORT_SELECT("ABORT("#ret");", true, (ret), __VA_ARGS__)

 /**
  * @def ABORT_IF_NOT(cond, ret, ...)
  *
  * Triggers an abort in the event that the specified condition \a cond
  * is false. This will cause the currently executing function to exit
  * with the return value \a ret. Any additional arguments will be used
  * to construct an error message
  */
#define ABORT_IF_NOT(cond, ret, ...) \
    ABORT_SELECT("ABORT_IF_NOT("#cond", "#ret");", !(cond), (ret), __VA_ARGS__)

#endif  // ABORT_H_
