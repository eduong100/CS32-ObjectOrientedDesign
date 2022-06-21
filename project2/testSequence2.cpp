#include "Sequence.h"
#include <type_traits>

#define CHECKTYPE(c, f, r, a)  \
    {  \
        static_assert(std::is_same<decltype(&c::f), r (c::*)a>::value, \
            "You did not declare " #c "::" #f " as the spec does");  \
        r (c::*p) a = &c::f;  \
        (void) p;  \
    }
#define CHECKTYPENONMEMBER(f, r, a)  \
    {  \
        static_assert(std::is_same<decltype(&f), r (*)a>::value, \
            "You did not declare " #f " as the spec does");  \
        r (*p) a = f;  \
        (void) p;  \
    }

static_assert(std::is_default_constructible<Sequence>::value,
    "Sequence must be default-constructible.");
static_assert(std::is_copy_constructible<Sequence>::value,
    "Sequence must be copy-constructible.");
static_assert(std::is_copy_assignable<Sequence>::value,
    "Sequence must be assignable.");

void thisFunctionWillNeverBeCalled()
{
    CHECKTYPE(Sequence, empty, bool, () const);
    CHECKTYPE(Sequence, size, int, () const);
    CHECKTYPE(Sequence, erase, bool, (int));
    CHECKTYPE(Sequence, remove, int, (const ItemType&));
    CHECKTYPE(Sequence, get, bool, (int, ItemType&) const);
    CHECKTYPE(Sequence, set, bool, (int, const ItemType&));
    CHECKTYPE(Sequence, find, int, (const ItemType&) const);
    CHECKTYPE(Sequence, swap, void, (Sequence&));
    CHECKTYPENONMEMBER(subsequence, int, (const Sequence&, const Sequence&));
    CHECKTYPENONMEMBER(interleave, void, (const Sequence&, const Sequence&, Sequence&));
    { auto p = static_cast<int (Sequence::*)(int, const ItemType&)>(&Sequence::insert); (void) p; }
    { auto p = static_cast<int (Sequence::*)(const ItemType&)>(&Sequence::insert); (void) p; }
}

int main()
{}