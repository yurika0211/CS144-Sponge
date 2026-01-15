#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _next_index(0), buffer() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    if (data.empty() && !eof)
        return;

    uint64_t start = std::max(index, _next_index);
    if (start < index + data.size()) {
        std::string sub = data.substr(start - index);
        buffer[start] = sub;
    }

    // 尝试写入_output
    while (!buffer.empty()) {
        auto it = buffer.begin();
        if (it->first != _next_index)
            break;
        _output.write(it->second);
        _next_index += it->second.size();
        buffer.erase(it);
    }

    if (eof)
        _output.end_input();
}
size_t StreamReassembler::unassembled_bytes() const {
    size_t count = 0;
    for (const auto &p : buffer) {
        count += p.second.size();
    }
    return count;
}

bool StreamReassembler::empty() const {
    return buffer.empty();
}