#define TTL_ENABLE_TEST
#include <ttl.hpp>

int
main() {
  if (!::ttl::test::run_tests())
    return 1;
}
