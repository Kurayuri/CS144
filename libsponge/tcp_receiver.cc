#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    DUMMY_CODE(seg);

    const TCPHeader &header = seg.header();
    if (!header.syn && !_syned) {
        return;
    }

    string data = seg.payload().copy();
    bool eof = false;

    if (header.syn && !_syned) {
        _syned = true;
        _isn = header.seqno;
        if (header.fin) {
            _fined = eof = true;
        }
        _reassembler.push_substring(data, 0, eof);
    }

    if (_syned && header.fin) {
        _fined = eof = true;
    }

    uint64_t checkpoint = _reassembler.assembled_bytes();
    uint64_t abs_seqno = unwrap(header.seqno, _isn, checkpoint);
    uint64_t stream_index = abs_seqno - 1;

    _reassembler.push_substring(data, stream_index, eof);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_syned) {
        return nullopt;
    }

    // Assembled_bytes + SYN + (FIN)
    size_t abs_seqno = _reassembler.assembled_bytes() + 1 + (_fined && _reassembler.empty());
    return wrap(abs_seqno, _isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
