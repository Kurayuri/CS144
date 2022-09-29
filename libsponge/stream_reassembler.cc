#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _buffer(capacity), _buffer_bitmap(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    DUMMY_CODE(data, index, eof);

    size_t data_size{data.size()};
    if (eof) {
        _eof = true;
    }
    if (data_size == 0 && _eof && _unassembled_bytes == 0) {
        _output.end_input();
        return;
    }

    if (index >= _assembled_bytes + _capacity - _output.buffer_size()) {
        return;
    } else if (index >= _assembled_bytes) {
        size_t offset{index - _assembled_bytes};
        size_t inputable_size{min(data_size, _output.remaining_capacity() - offset)};
        if (inputable_size < data_size) {
            _eof = false;
        }
        for (size_t i{0}; i < inputable_size; ++i) {
            if (!_buffer_bitmap[i + offset]) {
                ++_unassembled_bytes;
            }
            _buffer[i + offset] = data[i];
            _buffer_bitmap[i + offset] = true;
        }
    } else if (index + data_size > _assembled_bytes) {
        size_t offset{_assembled_bytes - index};
        size_t inputable_size{min(data_size - offset, _output.remaining_capacity())};
        if (inputable_size < data_size - offset) {
            _eof = false;
        }
        for (size_t i{0}; i < inputable_size; ++i) {
            if (!_buffer_bitmap[i]) {
                ++_unassembled_bytes;
            }
            _buffer[i] = data[i + offset];
            _buffer_bitmap[i] = true;
        }
    }

    size_t pushable_size{0};

    while (_buffer_bitmap.front()) {
        ++pushable_size;
        _buffer_bitmap.pop_front();
        _buffer_bitmap.push_back(false);
    }

    if (pushable_size > 0) {
        _output.write(string(_buffer.begin(), _buffer.begin() + pushable_size));
        _assembled_bytes += pushable_size;
        _unassembled_bytes -= pushable_size;
        _buffer.erase(_buffer.begin(), _buffer.begin() + pushable_size);
        _buffer.resize(_capacity);
    }
    if (_eof && _unassembled_bytes == 0) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _unassembled_bytes == 0; }