namespace collections {

  namespace array {
    using ::ttl::traits::IMPLEMENTS;
    using ::ttl::traits::CapacityPolicy;
    using ::ttl::storage::Chunk;
    using ::ttl::traits::Stack;

    template <typename T, typename P = DefaultResizingPolicy, typename = void>
    struct Array;

    template <typename T, typename P>
    struct Array<T, P, IMPLEMENTS<P, CapacityPolicy>> {
      ::std::size_t size;
      Chunk<T> data;

      Array(::std::size_t capacity)
          : size(0)
          , data(CapacityPolicy::initial<P>(capacity)) {
      }

      Array(Array &&o) noexcept : size(0), data(::std::move(o.data)) {
        ::std::swap(size, o.size);
      }

      ~Array() {
        while (!Stack::is_empty(*this))
          Stack::pop(*this);
      }
    };
  }

  using array::Array;
}

namespace traits {

  template <typename T, typename P>
  struct Collection::Impl<::ttl::collections::Array<T, P>, void> {
  private:
    using Array = ::ttl::collections::Array<T, P>;

  public:
    static ::std::size_t
    size(Array const &self) {
      return self.size;
    }
  };

  template <typename T>
  struct Bounded::Impl<
      ::ttl::collections::Array<T, ::ttl::collections::FixedCapacity>, void> {
  private:
    using Array =
        ::ttl::collections::Array<T, ::ttl::collections::FixedCapacity>;

  public:
    static ::std::size_t
    capacity(Array const &self) {
      return self.data.capacity;
    }
  };

  template <typename T, typename P>
  struct Unbounded::Impl<::ttl::collections::Array<T, P>,
                         IMPLEMENTS<P, ResizingPolicy>> {
  private:
    using Array = ::ttl::collections::Array<T, P>;

  public:
    static ::std::size_t
    capacity(Array const &self) {
      return self.data.capacity;
    }

    static void
    reserve(Array &self, ::std::size_t capacity) {
      if (capacity > self.data.capacity)
        self.data.resize(capacity);
    }

    static void
    shrink_to_fit(Array &self) {
      ::std::size_t capacity = CapacityPolicy::initial<P>(self.size);
      if (capacity < self.data.capacity)
        self.data.resize(capacity);
    }
  };

  template <typename T, typename P>
  struct List::Impl<::ttl::collections::Array<T, P>, void> {
  private:
    using Array = ::ttl::collections::Array<T, P>;

  public:
    using Item = T;

    static T const &
    get(Array const &self, ::std::size_t index) {
      ASSERT(index < self.size);
      return self.data.get(index);
    }
  };

  template <typename T, typename P>
  struct ListMut::Impl<::ttl::collections::Array<T, P>, void> {
  private:
    using Array = ::ttl::collections::Array<T, P>;

  public:
    using Item = T;

    static T &
    get(Array &self, ::std::size_t index) {
      ASSERT(index < self.size);
      return self.data.get(index);
    }
  };

  template <typename T>
  struct Stack::Impl<
      ::ttl::collections::Array<T, ::ttl::collections::FixedCapacity>, void> {
  private:
    using Array =
        ::ttl::collections::Array<T, ::ttl::collections::FixedCapacity>;

  public:
    using Item = T;

    static bool
    is_empty(Array const &self) {
      return self.size == 0;
    }

    static void
    push(Array &self, T &&item) {
      ASSERT(self.size < self.data.capacity);
      self.data.write(self.size++, ::std::move(item));
    }

    static T
    pop(Array &self) {
      ASSERT(self.size > 0);
      return self.data.read(--self.size);
    }
  };

  template <typename T, typename P>
  struct Stack::Impl<::ttl::collections::Array<T, P>,
                     IMPLEMENTS<P, ResizingPolicy>> {
  private:
    using Array = ::ttl::collections::Array<T, P>;

  public:
    using Item = T;

    static bool
    is_empty(Array const &self) {
      return self.size == 0;
    }

    static void
    push(Array &self, T &&item) {
      if (self.size == self.data.capacity)
        self.data.resize(ResizingPolicy::grow<P>(self.size));

      self.data.write(self.size++, ::std::move(item));
    }

    static T
    pop(Array &self) {
      ASSERT(self.size > 0);

      T item = self.data.read(--self.size);
      ::std::size_t capacity = self.data.capacity;
      ::std::size_t new_capacity =
          ResizingPolicy::shrink<P>(self.size, capacity);

      if (new_capacity < capacity)
        self.data.resize(new_capacity);

      return item;
    }
  };
}

#ifdef TTL_ENABLE_TEST
namespace collections {
  namespace array {
    namespace {
      using ::ttl::test::test_stack_destruction;
      using ::ttl::test::test_stack;
      using ::ttl::test::test_bounded_stack_overflow;
      using ::ttl::test::test_unbounded_stack_grow;
      using ::ttl::test::test_unbounded_stack_shrink;
      using ::ttl::test::test_unbounded_reserve;
      using ::ttl::test::test_unbounded_shrink_to_fit;

      template <typename T> using FixedArray = Array<T, FixedCapacity>;

      TESTCASE("test array") {

        SECTION("destruction") {
          SECTION("fixed") {
            test_stack_destruction<FixedArray>({10});
          }

          SECTION("resizing") {
            test_stack_destruction<Array>({10});
          }
        }

        SECTION("stack") {
          SECTION("fixed") {
            test_stack<FixedArray>({10});
          }
          SECTION("resizing") {
            test_stack<Array>({10});
          }
        }

        SECTION("bounded") {
          test_bounded_stack_overflow<FixedArray>({5});
        }

        SECTION("unbounded stack") {
          SECTION("grow") {
            test_unbounded_stack_grow<Array>({0});
          }
          SECTION("shrink") {
            test_unbounded_stack_shrink<Array>({0});
          }
        }

        SECTION("unbounded") {
          Array<::std::size_t> a(0);

          SECTION("reserve") {
            test_unbounded_reserve(a);
          }
          SECTION("shrink_to_fit") {
            for (::std::size_t i = 0; i < 11; i++)
              Stack::push(a, ::std::move(i));

            test_unbounded_shrink_to_fit(a);

            for (::std::size_t i = 0; i < 11; i++)
              ASSERT(Stack::pop(a) == 10 - i);
          }
        }
      }
    }
  }
}
#endif
