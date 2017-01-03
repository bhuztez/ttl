namespace storage {

  template <typename T, typename = void> struct Pool;

  template <typename T>
  struct Pool<
      T, typename ::std::enable_if<::std::is_nothrow_move_constructible<
             T>::value && ::std::is_nothrow_destructible<T>::value>::type> {
    ::std::size_t capacity;
    T *data;
    ::std::size_t next;
    T *empty;

    Pool(Pool const &) = delete;
    Pool &
    operator=(Pool const &) = delete;

    Pool(Pool &&o) noexcept : capacity(0),
                              data(nullptr),
                              next(0),
                              empty(nullptr) {
      ::std::swap(capacity, o.capacity);
      ::std::swap(data, o.data);
      ::std::swap(next, o.next);
      ::std::swap(empty, o.empty);
    }

    Pool(::std::size_t capacity)
        : capacity(capacity)
        , data(nullptr)
        , next(0)
        , empty(nullptr) {
      data = static_cast<T *>(::std::malloc(
          ::std::max(sizeof(T), sizeof(::std::uintptr_t)) * capacity));
    }

    T *
    get_ptr(::std::size_t index) {
      if (sizeof(T) < sizeof(::std::uintptr_t)) {
        return (T *)(((::std::uintptr_t *)data) + index);
      } else {
        return data + index;
      }
    }

    ~Pool() {
      if (data != nullptr)
        ::std::free(data);
    }
  };
}

namespace traits {

  template <typename T> struct Bounded::Impl<::ttl::storage::Pool<T>> {
  private:
    using Pool = ::ttl::storage::Pool<T>;

  public:
    static ::std::size_t
    capacity(Pool const &self) {
      return self.capacity;
    }
  };

  template <typename T> struct Allocator::Impl<::ttl::storage::Pool<T>> {
  private:
    using Pool = ::ttl::storage::Pool<T>;

  public:
    using Item = T;

    static Item *
    add(Pool &self, Item &&item) {
      Item *ptr = nullptr;
      if (self.empty != nullptr) {
        ptr = self.empty;
        self.empty = *((T **)ptr);
      } else {
        ASSERT(self.next < self.capacity);
        ptr = self.get_ptr(self.next++);
      }
      new (ptr) Item(::std::move(item));
      return ptr;
    }

    static Item
    remove(Pool &self, Item *ptr) {
      ASSERT((ptr >= self.get_ptr(0)) && (ptr < self.get_ptr(self.capacity)));
      Item item(::std::move(*ptr));
      ptr->~Item();

      *((T **)ptr) = self.empty;
      self.empty = ptr;
      return item;
    }
  };
}

#ifdef TTL_ENABLE_TEST
namespace storage {
  namespace pool {
    namespace {
      using ::ttl::test::AssertionFailure;
      using ::ttl::traits::Allocator;
      using ::ttl::test::test_allocator_item_destruction;

      TESTCASE("test pool") {
        test_allocator_item_destruction<Pool>({1});

        SECTION("full") {
          Pool<::std::size_t> pool(1);
          Allocator::add(pool, 1);
          ASSERT_THROW(AssertionFailure, Allocator::add(pool, 1));
        }

        SECTION("uintptr_t") {
          Pool<::std::uintptr_t> pool(3);
          ::std::uintptr_t *item1, *item2, *item3;
          item1 = Allocator::add(pool, 1);
          ASSERT(1 == *item1);
          Allocator::remove(pool, item1);
          item2 = Allocator::add(pool, 2);
          ASSERT(2 == *item2);
          ASSERT(item1 == item2);
          Allocator::remove(pool, item2);

          item1 = Allocator::add(pool, 1);
          item2 = Allocator::add(pool, 2);
          item3 = Allocator::add(pool, 3);
          ASSERT(1 == *item1);
          ASSERT(2 == *item2);
          ASSERT(3 == *item3);
          Allocator::remove(pool, item1);
          Allocator::remove(pool, item2);
          Allocator::remove(pool, item3);
          ASSERT(item3 == Allocator::add(pool, 3));
          ASSERT(item2 == Allocator::add(pool, 2));
          ASSERT(item1 == Allocator::add(pool, 1));
          ASSERT(1 == *item1);
          ASSERT(2 == *item2);
          ASSERT(3 == *item3);
        }

        SECTION("uint8_t") {
          Pool<::std::uint8_t> pool(3);
          ::std::uint8_t *item1, *item2, *item3;
          item1 = Allocator::add(pool, 1);
          ASSERT(1 == *item1);
          Allocator::remove(pool, item1);
          item2 = Allocator::add(pool, 2);
          ASSERT(2 == *item2);
          ASSERT(item1 == item2);
          Allocator::remove(pool, item2);

          item1 = Allocator::add(pool, 1);
          item2 = Allocator::add(pool, 2);
          item3 = Allocator::add(pool, 3);

          ASSERT(item1 + sizeof(uintptr_t) == item2);
          ASSERT(item2 + sizeof(uintptr_t) == item3);

          ASSERT(1 == *item1);
          ASSERT(2 == *item2);
          ASSERT(3 == *item3);
          Allocator::remove(pool, item1);
          Allocator::remove(pool, item2);
          Allocator::remove(pool, item3);
          ASSERT(item3 == Allocator::add(pool, 3));
          ASSERT(item2 == Allocator::add(pool, 2));
          ASSERT(item1 == Allocator::add(pool, 1));
          ASSERT(1 == *item1);
          ASSERT(2 == *item2);
          ASSERT(3 == *item3);
        }
      }
    }
  }
}
#endif
