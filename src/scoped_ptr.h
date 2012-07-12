#ifndef _INCLUDE_SCOPED_PTR_
#define _INCLUDE_SCOPED_PTR_

class noncopyable
{
public:
    noncopyable() {}
    virtual ~noncopyable() {}

private:
    noncopyable(const noncopyable &);
    noncopyable & operator=(const noncopyable &);
};

template <typename T>
class scoped_ptr : noncopyable
{
public:
    typedef T element_type;

    explicit scoped_ptr(T * ptr = NULL) : _ptr(ptr) {}
    ~scoped_ptr() { delete _ptr; }

    T & operator*() const { return *_ptr; }
    T * operator->() const { return _ptr; }
    T * get() const { return _ptr; }
    bool operator!() const { return _ptr == NULL; }

private:
    element_type * _ptr;
};

template <typename T>
class scoped_array : noncopyable
{
public:
    typedef T element_type;

    explicit scoped_array(T * ptr = NULL) : _ptr(ptr) {}
    ~scoped_array() { delete [] _ptr; }

    const T & operator[](std::ptrdiff_t i) const { return _ptr[i]; }
    T & operator[](std::ptrdiff_t i) { return _ptr[i]; }
    T * get() const { return _ptr; }
    bool operator!() const { return _ptr == NULL; }

private:
    element_type * _ptr;
};

#endif //_INCLUDE_SCOPED_PTR_
