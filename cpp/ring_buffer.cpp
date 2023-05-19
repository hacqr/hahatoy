#include <assert.h>
#include <memory.h>
#include <limits.h>
#include "ring_buffer.h"

#define MIN(a,b) ((a)<(b)?(a):(b))

#include <iostream>

ring_buffer::ring_buffer(unsigned int n)
    : _rp(0)
    , _wp(0)
{
    n--; 
    n|=n>>1; n|=n>>2; n|=n>>4; n|=n>>8; n|=n>>16; 
    n++;
    n = (unsigned int)(n==0);
    _ptr = new unsigned char[n];
    _size = n;
    std::cout << "size is " << n << std::endl;
}

ring_buffer::~ring_buffer()
{
    _rp = _wp = 0;
    delete [] _ptr;
    _ptr = nullptr;
}

int ring_buffer::put_forward(int n)
{
    assert(plen() >= n);
    _wp += n;
    return 0;
}

int ring_buffer::get_forward(int n)
{
    assert(glen() >= n);
    _rp += n;
    return 0;
}

int ring_buffer::plen() const
{
    const pos_t rp = map_pos(_rp);
    const pos_t wp = map_pos(_wp);

    if (wp > rp) {
        return _size - wp + rp;
    } else if (wp < rp) {
        return rp - wp;
    } else {
        return _wp!=_rp ? 0 : _size;
    }

    return 0;
}

int ring_buffer::glen() const
{
    const pos_t rp = map_pos(_rp);
    const pos_t wp = map_pos(_wp);

    if (wp > rp) {
        return wp - rp;
    } else if (wp < rp) {
        return _size - rp + wp;
    } else {
        return _wp!=_rp ? _size : 0;
    }

    return 0;
}

int ring_buffer::pptr(unsigned char** pptr, int* plen) const {
    const pos_t rp = map_pos(_rp);
    const pos_t wp = map_pos(_wp);

    int buf_seg = 0;

    if (wp > rp) {
        pptr[0] = _ptr + wp; plen[0] = _size - wp;
        pptr[1] = _ptr; plen[1] = rp;
        buf_seg = rp==0 ? 1 : 2;
    } else if (wp < rp) {
        pptr[0] = _ptr + wp; plen[0] = rp - wp;
        buf_seg = 1;
    } else {
        if (_wp == _rp) {
            pptr[0] = _ptr + wp; plen[0] = _size - wp;
            pptr[1] = _ptr; plen[1] = rp;
            buf_seg = rp==0 ? 1 : 2;
        }
    }
    return buf_seg;
}

int ring_buffer::gptr(unsigned char** pptr, int* plen) const {
    const pos_t rp = map_pos(_rp);
    const pos_t wp = map_pos(_wp);

    int buf_seg = 0;

    if (wp > rp) {
        pptr[0] = _ptr + rp; plen[0] = wp - rp;
        buf_seg = 1;
    } else if (wp < rp) {
        pptr[0] = _ptr + rp; plen[0] = _size - rp;
        pptr[1] = _ptr; plen[1] = wp;
        buf_seg = wp==0 ? 1: 2;
    } else {
        if (_wp != _rp) {
            pptr[0] = _ptr + rp; plen[0] = _size - rp;
            pptr[1] = _ptr; plen[1] = rp;
            buf_seg = rp==0 ? 1: 2;
        }
    }
    return buf_seg;
}

ring_buffer::pos_t ring_buffer::map_pos(pos_t n) const
{
    return n % _size;
}

unsigned int ring_buffer::size() const
{
    return _size;
}

int ring_buffer::put(unsigned char* p, int len)
{
    unsigned char* pp[2];
    int ll[2] = {0};

    int write_len = 0;
    int n = pptr(pp, ll);

    if (n == 0) {
        return -1;
    }

    write_len = MIN(len, ll[0]);
    ::memcpy(pp[0], p, write_len);

    len -= write_len;

    if (len > 0 && n > 1) {
        len = MIN(len, ll[1]);
        ::memcpy(pp[1], p+write_len, len);
        write_len += len;
    }

    put_forward(write_len);

    return write_len;
}

int ring_buffer::get(unsigned char* p, int len)
{
    unsigned char* pp[2];
    int ll[2] = {0};

    int read_len = 0;
    int n = gptr(pp, ll);

    if (n == 0) {
        return -1;
    }

    read_len = MIN(len, ll[0]);
    ::memcpy(p, pp[0], read_len);

    len -= read_len;

    if (len > 0 && n > 1) {
       len = MIN(len, ll[1]);
       ::memcpy(p+read_len, pp[1], len);
       read_len += len;
    }

    get_forward(read_len);

    return read_len;
}

int ring_buffer::set_size(int n)
{
    //Not implement
    return 0;
}
