#ifndef _INCLUDE_CACHE_
#define _INCLUDE_CACHE_

#include "common.h"

template <typename TYPE>
class FIFOCache
{
public:
    typedef TYPE index_type;

    FIFOCache(UInt8 size)
    : _cache_size(size), _front(0), _end(0),
      _miss_count(0), _load_count(0)
    {
        _cache = new index_type[_cache_size];
    }

    ~FIFOCache()
    {
        delete [] _cache;
    }

    void load(index_type index)
    {
        ++_load_count;

        for (UInt8 i = _front; i != _end; i = next(i))
        {
            if (_cache[i] == index)
                return;
        }

        ++_miss_count;
        if (full())
            _front = next(_front);
        _cache[_end] = index;
        _end = next(_end);
    }

    template <typename Iter>
    void load(Iter begin, Iter end)
    {
        for (Iter iter = begin; iter != end; ++iter)
        {
            load(*iter);
        }
    }

    UInt32 miss_count() const { return _miss_count; }
    UInt32 load_count() const { return _load_count; }

private:
    UInt8 next(UInt8 i) const
    {
        if (++i == _cache_size)
            i = 0;
        return i;
    }

    bool empty() const { return _front == _end; }
    bool full() const { return next(_end) == _front; }

    index_type * _cache;
    UInt8 _cache_size;
    UInt8 _front;
    UInt8 _end;

    UInt32 _miss_count;
    UInt32 _load_count;
};

#endif // _INCLUDE_CACHE_
