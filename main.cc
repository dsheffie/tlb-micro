#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include "entry.hh"
#include "perf.hh"

template <typename T>
void swap(T &x, T &y) {
  T t = x; x = y; y = t;
}

template <typename T>
void shuffle(std::vector<T> &vec) {
  for(size_t i = 0, l = vec.size(); i < l; i++) {
    size_t j = i + rand() % (l-i);
    swap<T>(vec[i], vec[j]);
  }
}

static void build_list(entry *list, size_t n_entries) {
  std::vector<size_t> vec(n_entries);
  for(size_t i = 0; i < n_entries; i++) {
    vec[i] = i;
  }
  shuffle(vec);
  entry *h = list+vec[0];
  entry *c = h;
  h->next = h;
  for(size_t i = 1; i < n_entries; i++) {
    entry *n = list+vec[i];
    n->next = c->next;    
    c->next = n;
  }
}

int main(int argc, char *argv[]) {

  static const int pgsize = getpagesize();
  uint64_t lg2_start = 4, lg2_stop = 16;
  int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  static const size_t min_work = 1UL<<24;



  for(uint64_t n = (1UL<<lg2_start); n <= (1UL<<lg2_stop); n *= 2) {
    void *buf = nullptr;
    size_t length = sizeof(entry)*n;
    buf = mmap(nullptr, length, PROT_READ|PROT_WRITE, flags, -1, 0);
    entry *entries = reinterpret_cast<entry*>(buf);
    build_list(entries, n);

    size_t work = n * 32;
    work = (work < min_work) ? min_work : work;

    cycle_counter cc;
    cc.enable_counter();
    cc.reset_counter();
    uint64_t count = cc.read_counter();
    entry *w = walk(entries, work);
    count = cc.read_counter() - count;

    double avg_cycles = static_cast<double>(count) / (work); 
    std::cout << "size = " << length << ", pages = " << (length/pgsize)
	      << ", avg cycles = " << avg_cycles << "\n";
    munmap(buf, length);
  }
  
  return 0;
}
