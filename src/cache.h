#ifndef _INCLUDE_CACHE_
#define _INCLUDE_CACHE_

#include "common.h"

#include <list>

// A FIFO cache
template <typename TYPE>
class FIFOCache
{
public:
    typedef TYPE index_type;

    FIFOCache(UInt8 size)
    : cache_size(size), miss_count(0), load_count(0)
    {
    }

    void load(index_type index)
    {
        ++load_count;

        typename std::list<index_type>::const_iterator citer = cache.begin();
        for (; citer != cache.end(); ++citer)
        {
            if (*citer == index)
                return;
        }

        ++miss_count;
        if (cache.size() > cache_size)
            cache.pop_front();
        cache.push_back(index);
    }

    template <typename Iter>
    void load(Iter begin, Iter end)
    {
        for (Iter iter = begin; iter != end; ++iter)
        {
            load(*iter);
        }
    }

    UInt32 get_miss_count() const { return miss_count; }
    UInt32 get_load_count() const { return load_count; }

private:
    std::list<index_type> cache;
    UInt8 cache_size;
    UInt32 miss_count;
    UInt32 load_count;
};

#endif // _INCLUDE_CACHE_
