namespace collections {
  struct FixedCapacity {};

  struct DefaultResizingPolicy {};
}

namespace traits {
  template <>
  struct CapacityPolicy::Impl<::ttl::collections::FixedCapacity, void> {
    static ::std::size_t
    initial(::std::size_t capacity) {
      return capacity;
    }
  };

  template <>
  struct CapacityPolicy::Impl<::ttl::collections::DefaultResizingPolicy, void> {
    static ::std::size_t
    initial(::std::size_t capacity) {
      return ::std::max(::std::size_t(10u), capacity);
    }
  };

  template <>
  struct ResizingPolicy::Impl<::ttl::collections::DefaultResizingPolicy, void> {
    static ::std::size_t
    grow(::std::size_t capacity) {
      return capacity + (capacity / 2u);
    }

    static ::std::size_t
    shrink(::std::size_t size, ::std::size_t capacity) {
      ::std::size_t new_capacity;

      if (size * ::std::size_t(9u) / ::std::size_t(4u) < capacity) {
        new_capacity = size * ::std::size_t(3u) / ::std::size_t(2u);
      } else {
        new_capacity = capacity;
      }

      return ::std::max(::std::size_t(10u), new_capacity);
    }
  };
}
