#ifndef SRC_MACROS_H_
#define SRC_MACROS_H_

#define CHECK(x)                                                \
  do {                                                          \
    if (!(x)) {                                                 \
      printf("%s:%d: Error in function %s", __FILE__, __LINE__, \
             __PRETTY_FUNCTION__);                              \
      abort();                                                  \
    }                                                           \
  } while (false)

#ifndef NDEBUG
#define DCHECK(x) CHECK(true)
#elif
#define DCHECK(x) CHECK(x)
#endif

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
  ClassName(const ClassName&) = delete;     \
  ClassName& operator=(const ClassName&) = delete

#endif  // SRC_MACROS_H_
