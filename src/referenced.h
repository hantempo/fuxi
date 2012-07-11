#ifndef _INCLUDE_REFERENCED_
#define _INCLUDE_REFERENCED_

#include "common.h"

class Referenced
{
public:
    Referenced() : ref_count(0) {}

    void ref() { ++ref_count; }
    void unref() { --ref_count; }
    UInt32 reference_count() const { return ref_count; }
    
private:
    UInt32 ref_count;
};

#endif // _INCLUDE_REFERENCED_
