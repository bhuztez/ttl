namespace test {
  using ::ttl::traits::Allocator;
  using ::ttl::traits::IMPLEMENTS;

  template <template <typename...> typename T>
  IMPLEMENTS<T<Counter>, Allocator>
  test_allocator_item_destruction(T<Counter> &&allocator) {
    Counter::count = 0;
    Counter *ptr;

    {
      ptr = Allocator::add(allocator, {});
      ASSERT(Counter::count == 0);
    }

    {
      Counter c = Allocator::remove(allocator, ptr);
      ASSERT(Counter::count == 0);
    }

    ASSERT(Counter::count == 1);
  }
}
