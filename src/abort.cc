/**
 *  \file   abort.cc
 *  \author Jason Fernandez
 *  \date   12/28/2019
 *
 *  https://github.com/jfern2011/abort
 */

#include "abort/abort.h"

#include <iostream>

namespace diagnostics {
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
 * The currently executing function's distance to the bottom of the call stack
 */
int frame_cnt = 0;

/**
 * Write an abort message to the output stream
 *
 * @param[in] select    The selected macro
 * @param[in] num_args  Number of macro arguments passed
 * @param[in] cond      The condition that triggered the abort
 * @param[in] ret       The function/method return value
 * @param[in] frame_cnt Current distance to the bottom of the call stack
 * @param[in] file      The name of the file from which this message originated
 * @param[in] line      The line number at which this message originated
 * @param[in] func      Function (or method) from which this message originated
 * @param[in] msg       A custom message passed to an abort macro
 *
 */
void print_msg(const char* select, int num_args, const char* cond,
               const char* ret, int frame_cnt, const char* file, int line,
               const char* func, const std::string& msg) {

    get_ostream() << "abort[" << frame_cnt << "]: " << file << ":" << line
                  << ": In '" << func << "':";

    if (num_args == 0) {
        get_ostream() << " " << select << "(" << cond
                      << (std::string(cond).empty() ? "" : ", ") << ret << ");"
                      << std::endl;
    } else {
        get_ostream() << msg << std::endl;
    }
}

}  // namespace internal

/**
 * Get the stream object currently being written to
 *
 * @return The output stream
 */
std::ostream& get_ostream() {
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
void set_message_size(std::size_t size) {
    // Add 1 to account for the leading space in call to snprintf()
    // and the null terminator
    internal::buffer.resize(size+2);
}

/**
 * Set the stream object to write to. By default, messages are written
 * to standard output
 *
 * @return The output stream
 */
void set_ostream(std::shared_ptr<std::ostream> os) {
    internal::stream = os;
}

}  // namespace diagnostics
