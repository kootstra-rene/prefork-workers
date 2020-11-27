#include <cstdio>
#include <cstdlib>
#include <unordered_map>

static bool lazy = false;

template <class T>
class my_allocator {
public:
  typedef T value_type;

  my_allocator() {}
  my_allocator(const my_allocator&) {}

  T* allocate(size_t n, const void * = 0) {
    return (T*) malloc(n * sizeof(T));
  }
  
  void deallocate(void* block, size_t) {
    free(block);
  }

  template <class U> my_allocator(const my_allocator<U>&) {
  }
};

std::unordered_map<
  void*, 
  size_t, 
  std::hash<void*>, 
  std::equal_to<void*>, 
  my_allocator<std::pair<const void*, size_t> >
> umap;

static void checkForLeaks() {
  printf("\ncheckForLeaks\n");
  for (auto it = umap.begin(); it != umap.end(); ++it) {
    fprintf(stdout, "  %p: %ld bytes\n", it->first, it->second);
  }
}

static void* _malloc(size_t size) {
  if (!lazy) {
    atexit(checkForLeaks);
    lazy = true;
  }
  auto block = malloc(size);
  // fprintf(stdout, "new: %ld @%p\n", size, block);
  umap[block] = size;
  return block;
}

static void _free(void* block) {
  // fprintf(stdout, "del: @%p\n", block);
  umap.erase(block);
  free(block);
}

void* operator new(size_t size) {
  return _malloc(size);
}

void* operator new [](size_t size) {
  return _malloc(size);
}

void operator delete(void* block) {
  _free(block);
}

void operator delete(void* block, size_t) {
  _free(block);
}

void operator delete [](void* block) {
  _free(block);
}

void operator delete [](void* block, size_t) {
  _free(block);
}