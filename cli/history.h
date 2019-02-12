/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2016-2019 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifndef CLI_DETAIL_HISTORY_H_
#define CLI_DETAIL_HISTORY_H_

#include <deque>
#include <string>

namespace cli
{
namespace detail
{

class History
{
public:

    explicit History(std::size_t size) : maxSize(size) {}

    // Insert a new item in the buffer, changing the current state to "inserting"
    // If we're browsing the history (eg with arrow keys) the new item overwrite
    // the current one.
    // Otherwise, the item is added to the front of the container
    void NewCommand(const std::string& item)
    {
        current = 0;
        if (mode == Mode::browsing)
        {
            assert(!buffer.empty());
            buffer[current] = item;
        }
        else // Mode::inserting
        {
            buffer.push_front(item);
            if (buffer.size() > maxSize)
                buffer.pop_back();
        }
        mode = Mode::inserting;
    }

    // Return the previous item of the history, updating the current item and
    // changing the current state to "browsing"
    // If we're already browsing the history (eg with arrow keys) the edit line is inserted
    // to the front of the container.
    // Otherwise, the line overwrites the current item.
    std::string Previous(const std::string& line)
    {
        if (mode == Mode::inserting)
        {
            buffer.push_front(line);
            if (buffer.size() > maxSize) buffer.pop_back();
            mode = Mode::browsing;
            current = (buffer.size() > 1) ? 1 : 0;
        }
        else // Mode::browsing
        {
            assert(!buffer.empty());
            buffer[current] = line;
            if (current != buffer.size()-1)
                ++current;
        }
        assert(mode == Mode::browsing);
        assert(current < buffer.size());
        return buffer[current];
    }

    // Return the next item of the history, updating the current item.
    std::string Next()
    {
        if (current != 0)
            --current;
        assert(current < buffer.size());
        return buffer[current];
    }

    // Show the whole history on the given ostream
    void Show(std::ostream& out) const
    {
        out << '\n';
        for ( auto& item: buffer )
            out << item << '\n';
        out << '\n' << std::flush;
    }

private:

    const std::size_t maxSize;
    std::deque<std::string> buffer;
    std::size_t current = 0;
    enum class Mode { inserting, browsing };
    Mode mode = Mode::inserting;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_HISTORY_H_
