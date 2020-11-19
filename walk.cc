#include "entry.hh"

entry *walk(entry *e, size_t x) {
  while(x) {
    e = e->next;
    --x;
  }
  return e;
}
