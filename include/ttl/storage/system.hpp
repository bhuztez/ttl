namespace storage {

  template <typename T, typename = void> struct SystemAllocator;

  template <typename T>
  struct SystemAllocator<
      T, typename ::std::enable_if<::std::is_nothrow_move_constructible<
             T>::value && ::std::is_nothrow_destructible<T>::value>::type> {
    SystemAllocator(SystemAllocator const &) = delete;
    SystemAllocator &
    operator=(SystemAllocator const &) = delete;

    SystemAllocator(SystemAllocator &&) = default;
  };
}

namespace traits {

  template <typename T>
  struct Allocator::Impl<::ttl::storage::SystemAllocator<T>> {
  private:
    using SystemAllocator = ::ttl::storage::SystemAllocator<T>;

  public:
    using Item = T;

    static Item *
    add(SystemAllocator &, Item &&item) {
      Item *ptr = static_cast<Item *>(::std::malloc(sizeof(Item)));
      new (ptr) Item(::std::move(item));
      return ptr;
    }

    static Item
    remove(SystemAllocator &, Item *ptr) {
      Item item(::std::move(*ptr));
      ptr->~Item();
      ::std::free(ptr);
      return item;
    }
  };
}

#ifdef TTL_ENABLE_TEST
namespace storage {
  namespace system {
    namespace {
      using ::ttl::test::test_allocator_item_destruction;

      TESTCASE("test system allocator") {
        test_allocator_item_destruction<SystemAllocator>({});
      }
    }
  }
}
#endif
