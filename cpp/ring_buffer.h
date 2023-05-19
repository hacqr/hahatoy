#pragma once

class ring_buffer {
    typedef unsigned int pos_t;
public:
    ring_buffer(unsigned int n);

    ~ring_buffer();

    unsigned int size() const;

    int set_size(int n);

    int put(unsigned char* p, int len);

    int get(unsigned char* p, int len);

private:
    int put_forward(int n);
    int get_forward(int n);

    int plen() const;
    int glen() const;

    int pptr(unsigned char** pptr, int* plen) const;

    int gptr(unsigned char** pptr, int* plen) const;

    pos_t map_pos(pos_t n) const;

private:
    pos_t _rp;
    pos_t _wp;
    unsigned char* _ptr;
    unsigned int _size;
};
