#ifndef ASSERT_H
#define ASSERT_H

// -- CAUTION --
// assert() is normally implemented in assert.h, but is untestable.
// This header is only intended to be used in seminars, do NOT use
// it otherwise and prefer standard assert.h instead.

#include <signal.h>

#define assert(expr)        \
    do {                    \
        if (!(expr)) {      \
            raise(SIGABRT); \
        }                   \
    } while (0)

#endif // ASSERT_H
