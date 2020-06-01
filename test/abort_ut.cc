/**
 *  \file   abort_ut.cc
 *  \author Jason Fernandez
 *  \date   12/28/2019
 *
 *  Copyright 2020 Jason Fernandez
 *
 *  https://github.com/jfern2011/abort
 */

#include <cstddef>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "abort/abort.h"

namespace {

/**
 * Split a string into tokens. A delimiter is specified to indicate
 * how to slice the string
 *
 * @param [in]  str    The string to split
 * @param [out] tokens A vector of the elements of the split string
 * @param [in]  delim  The delimiter
 */
void split(const std::string& str,
           std::vector<std::string>* tokens,
           const std::string& delim = " ") {
    const std::string line = str;

    tokens->clear();

    if (delim.empty()) { return; }

    size_t ind, start = 0;
    while ((ind= line.find(delim, start)) != std::string::npos) {
        if (ind - start > 0)
            tokens->push_back(line.substr(start, ind - start));
        start = ind + delim.size();
    }

    /* Final token */

    if (start < line.size())
        tokens->push_back(line.substr(start,
            std::string::npos));
}

/**
 * Remove leading and trailing whitespace from a string. This
 * includes the character set " \t\n\v\f\r"
 *
 * @param[in] str Input string
 *
 * @return     The input string with all leading and trailing
 *             whitespace removed
 */
std::string trim(const std::string& str) {
    if (str.empty()) return str;

    const std::string space = "\t\n\v\f\r ";

    size_t start =
        str.find_first_not_of(space);

    if (start == std::string::npos)
        return "";

    size_t stop = str.find_last_not_of(space);

    return
        str.substr(start, stop-start+1);
}

/**
 * ABORT* macro test fixture
 */
class AbortTest : public ::testing::Test {
 protected:
/** Test setup method */
void SetUp() override {
    stream_ = std::make_shared<std::ostringstream>();
}

/**
 * Writes the stack trace to the output stream via the ABORT_IF_NOT macro
 *
 * @param[in] depth The recursive depth limit
 *
 * @return -1 always
 */
int abort_if_not(int depth) {
    ABORT_IF_NOT(depth < 5, -1, "depth = %d", depth);
    ABORT_IF_NOT(abort_if_not(depth+1) == 0, -1);
}

/**
 * Writes the stack trace to the output stream via the ABORT macro
 *
 * @param[in] depth The recursive depth limit
 *
 * @return -1 always
 */
int abort(int depth) {
    if (depth >= 5) {
        ABORT(-1, "depth = %d", depth);
    } else {
        // Note: This prints the message first, then makes recursive call
        ABORT(abort(depth+1));
    }
}

/**
 * Writes the stack trace to the output stream via the ABORT_IF macro
 *
 * @param[in] depth The recursive depth limit
 *
 * @return -1 always
 */
int abort_if(int depth) {
    ABORT_IF(depth >= 5, -1, "depth = %d", depth);
    ABORT_IF(abort_if(depth+1) == -1, -1);
}

/**
 * The stream object to write to
 */
std::shared_ptr<std::ostringstream> stream_;
};

TEST_F(AbortTest, ABORT_IF_NOT) {
    ASSERT_TRUE(stream_);
    diagnostics::set_ostream(stream_);
    abort_if_not(0);

    std::string str = stream_->str();
    std::vector<std::string> lines;
    split(str, &lines, "\n");

    ASSERT_EQ(lines.size(), 6u);

    for (std::size_t i = 0; i < lines.size(); i++) {
        char preface[16];
        std::snprintf(preface, sizeof(preface),
                      "abort[%zu]:", lines.size()-i-1);
        EXPECT_EQ(lines[i].find(preface), 0);
        if (i == 0) {
            EXPECT_NE(lines[i].find("depth = 5"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        } else {
            EXPECT_NE(lines[i].find("ABORT_IF_NOT"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        }
    }
}

/**
 * @todo Frame counter is not very robust; in particular, it does not correctly
 *       print in ABORT() as it always decrements before yielding control
 */
TEST_F(AbortTest, ABORT) {
    ASSERT_TRUE(stream_);
    diagnostics::set_ostream(stream_);
    abort(0);

    std::string str = stream_->str();
    std::vector<std::string> lines;
    split(str, &lines, "\n");

    ASSERT_EQ(lines.size(), 6u);

    for (std::size_t i = 0; i < lines.size(); i++) {
        char preface[16];
        std::snprintf(preface, sizeof(preface),
                      "abort[%zu]:", std::size_t(0));
        EXPECT_EQ(lines[i].find(preface), 0);
        if (i == 5) {
            EXPECT_NE(lines[i].find("depth = 5"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        } else {
            EXPECT_NE(lines[i].find("ABORT"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        }
    }
}

TEST_F(AbortTest, ABORT_IF) {
    ASSERT_TRUE(stream_);
    diagnostics::set_ostream(stream_);
    abort_if(0);

    std::string str = stream_->str();
    std::vector<std::string> lines;
    split(str, &lines, "\n");

    ASSERT_EQ(lines.size(), 6u);

    for (std::size_t i = 0; i < lines.size(); i++) {
        char preface[16];
        std::snprintf(preface, sizeof(preface),
                      "abort[%zu]:", lines.size()-i-1);
        EXPECT_EQ(lines[i].find(preface), 0);
        if (i == 0) {
            EXPECT_NE(lines[i].find("depth = 5"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        } else {
            EXPECT_NE(lines[i].find("ABORT_IF"), std::string::npos)
                << "lines[" << i << "] = '" << std::string(lines[i]) << "'";
        }
    }
}

TEST_F(AbortTest, set_message_size) {
    ASSERT_TRUE(stream_);
    diagnostics::set_ostream(stream_);

    auto y = []() {
        ABORT(0, "hello");
    };

    for (std::size_t i = 0; i <= 5; i++) {
        diagnostics::set_message_size(i);

        y();

        std::string str = stream_->str();

        auto colonInd = str.rfind(":");     // message starts after ':'
        ASSERT_NE(colonInd, std::string::npos);

        auto substr = trim(str.substr(colonInd+1));
        EXPECT_EQ(substr.size()-1, i)       // excluding null terminator
            << "substr = " << substr;

        EXPECT_EQ(std::string("hello").substr(0, i),
                  substr.c_str());
    }
}

TEST_F(AbortTest, ABORT_ON_ERRNO) {
    ASSERT_TRUE(stream_);
    diagnostics::set_ostream(stream_);

    // Simulate a system call that returns -1 and sets errno to 5
    auto sys_call = []() {
        errno = 5;
        return -1;
    };

    auto y = [&]() {
        ABORT_ON_ERRNO(sys_call(), 0);
    };

    y();

    std::string str = stream_->str();

    auto colonInd = str.rfind(":");   // message starts after ':'
    ASSERT_NE(colonInd, std::string::npos);

    auto substr = trim(str.substr(colonInd+1));
    EXPECT_NE(substr.find("ABORT_ON_ERRNO"),
              std::string::npos);
}

}  // namespace
