namespace test {
  using ::ttl::traits::Collection;
  using ::ttl::traits::Unbounded;
  using ::ttl::traits::IMPLEMENTS;

  template <typename T>
  IMPLEMENTS<T, Unbounded>
  test_unbounded_reserve(T &u) {
    Unbounded::reserve(u, 20);
    ASSERT(Unbounded::capacity(u) == 20);
  }

  template <typename T>
  ::std::void_t<IMPLEMENTS<T, Unbounded>, IMPLEMENTS<T, Collection>>
  test_unbounded_shrink_to_fit(T &u) {
    ASSERT(Unbounded::capacity(u) > Collection::size(u));
    Unbounded::shrink_to_fit(u);
    ASSERT(Unbounded::capacity(u) == Collection::size(u));
  }
}
