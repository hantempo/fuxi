#ifndef _INCLUDE_REF_PTR_
#define _INCLUDE_REF_PTR_

template <typename T>
class ref_ptr
{
public:
    ref_ptr() : ptr(NULL)
    {}
    
    ref_ptr(T *p) : ptr(p)
    {
        ptr->ref();
    }

    ref_ptr(const ref_ptr &rp) : ptr(rp.ptr)
    {
        ptr->ref();
    }

    ~ref_ptr()
    {
        ptr->unref();
        if (ptr->reference_count() == 0)
            delete ptr;
    }

    ref_ptr & operator=(T *p)
    {
        if (p != ptr)
        {
            ptr->unref();
            if (ptr->reference_count() == 0)
                delete ptr;
            ptr = p;
            ptr->ref();
        }
    }

    ref_ptr & operator=(const ref_ptr &rp)
    {
        if (rp.ptr != ptr)
        {
            ptr->unref();
            if (ptr->reference_count() == 0)
                delete ptr;
            ptr = rp.ptr;
            ptr->ref();
        }
    }

    T & operator*() const
    {
        return *ptr;
    }

    T * operator->() const
    {
        return ptr;
    }

    T * get() const
    {
        return ptr;
    }

    bool operator!() const
    {
        return ptr != NULL;
    }

private:
    T *ptr;
};

#endif // _INCLUDE_REF_PTR_
