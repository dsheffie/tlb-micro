#ifndef __entryhh__
#define __entryhh__

#include <cstdint>
#include <cstddef>
#include <cstdlib>

struct entry {
  entry *next;
};

entry *walk(entry *, size_t);

#endif
