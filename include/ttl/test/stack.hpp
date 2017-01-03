namespace test {
  using ::ttl::traits::Stack;
  using ::ttl::traits::Bounded;
  using ::ttl::traits::Unbounded;
  using ::ttl::traits::IMPLEMENTS;

  template <template <typename...> typename T>
  IMPLEMENTS<T<Counter>, Stack>
  test_stack_destruction(T<Counter> &&stack) {
    Counter::count = 0;

    {
      T<Counter> s{::std::move(stack)};

      for (::std::size_t i = 0; i < 10; ++i)
        Stack::push(s, {});

      for (::std::size_t i = 0; i < 5; ++i) {
        { Stack::pop(s); }
        ASSERT(Counter::count == i + 1);
      }
    }

    ASSERT(Counter::count == 10);
  }

  template <template <typename...> typename T>
  IMPLEMENTS<T<::std::size_t>, Stack>
  test_stack(T<::std::size_t> &&s) {
    ASSERT(Stack::is_empty(s));

    for (::std::size_t i = 0; i < 5; i++)
      Stack::push(s, ::std::move(i));

    ASSERT(!(Stack::is_empty(s)));

    for (::std::size_t i = 0; i < 5; i++)
      ASSERT(Stack::pop(s) == 4 - i);

    ASSERT(Stack::is_empty(s));

    ASSERT_THROW(AssertionFailure, Stack::pop(s));
  }

  template <template <typename...> typename T>
  ::std::void_t<IMPLEMENTS<T<::std::size_t>, Stack>,
                IMPLEMENTS<T<::std::size_t>, Bounded>>
  test_bounded_stack_overflow(T<::std::size_t> &&s) {
    ASSERT(Stack::is_empty(s));
    ASSERT(Bounded::capacity(s) == 5);

    for (::std::size_t i = 0; i < 5; i++)
      Stack::push(s, ::std::move(i));

    ASSERT_THROW(AssertionFailure, Stack::push(s, 5));
  }

  template <template <typename...> typename T>
  ::std::void_t<IMPLEMENTS<T<::std::size_t>, Stack>,
                IMPLEMENTS<T<::std::size_t>, Unbounded>>
  test_unbounded_stack_grow(T<::std::size_t> &&s) {
    ASSERT(Stack::is_empty(s));

    for (::std::size_t i = 0; i < 10; i++)
      Stack::push(s, ::std::move(i));

    ASSERT(Unbounded::capacity(s) == 10);
    Stack::push(s, 10);
    ASSERT(Unbounded::capacity(s) > 10);
  }

  template <template <typename...> typename T>
  ::std::void_t<IMPLEMENTS<T<::std::size_t>, Stack>,
                IMPLEMENTS<T<::std::size_t>, Unbounded>>
  test_unbounded_stack_shrink(T<::std::size_t> &&s) {
    ASSERT(Stack::is_empty(s));

    for (::std::size_t i = 0; i < 11; i++)
      Stack::push(s, ::std::move(i));

    ::std::size_t capacity = Unbounded::capacity(s);

    for (::std::size_t i = 0; i < 5; i++)
      Stack::pop(s);

    ASSERT(Unbounded::capacity(s) < capacity);
  }
}
