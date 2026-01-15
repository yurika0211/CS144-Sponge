#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"
#include <map>
#include <cstdint>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes
    uint64_t _next_index = 0;                // 下一个要写入 _output 的字节索引
    std::map<uint64_t, std::string> buffer;  // 暂存乱序片段
  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};
#include "stream_reassembler.hh"
#include <algorithm>

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _next_index(0), buffer() { }

void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    if (data.empty() && !eof) return;

    uint64_t start = std::max(index, _next_index);
    if (start < index + data.size()) {
        std::string sub = data.substr(start - index);
        buffer[start] = sub;
    }

    // 尝试写入_output
    while (!buffer.empty()) {
        auto it = buffer.begin();
        if (it->first != _next_index) break;
        _output.write(it->second);
        _next_index += it->second.size();
        buffer.erase(it);
    }

    if (eof) _output.end_input();
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
#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
