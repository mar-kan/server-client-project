//Kanellaki Maria Anna - 1115201400060

#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H


class BoundedBuffer {
    int size;
    int * files;
    int start;
    int end;
    int count;

public:
    BoundedBuffer(int s);
    ~BoundedBuffer();
    void add(int);
    int remove();
    int getSize() const;
};


#endif