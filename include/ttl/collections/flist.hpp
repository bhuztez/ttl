namespace collections {
  namespace flist {
    using ::ttl::traits::IMPLEMENTS;
    using ::ttl::traits::Allocator;
    using ::ttl::traits::Stack;
    using ::ttl::storage::SystemAllocator;

    template <typename T> struct Node {
      Node<T> *next;
      T data;
    };

    template <typename T, typename A = SystemAllocator<Node<T>>,
              typename = void>
    struct ForwardList;

    template <typename T, typename A>
    struct ForwardList<
        T, A,
        ::std::void_t<
            IMPLEMENTS<A, Allocator>,
            typename ::std::enable_if<::std::is_nothrow_move_constructible<
                T>::value && ::std::is_nothrow_destructible<T>::value>::type>> {
      ::std::size_t size;
      Node<T> *top;
      A allocator;

      ForwardList()
          : size(0)
          , top(nullptr)
          , allocator({}) {
      }

      ForwardList(A &&a)
          : size(0)
          , top(nullptr)
          , allocator(::std::move(a)) {
      }

      ForwardList(ForwardList &&o) noexcept
          : size(0),
            top(nullptr),
            allocator(::std::move(o.allocator)) {
        ::std::swap(size, o.size);
        ::std::swap(top, o.top);
      }

      static ForwardList
      create() {
        return {0, nullptr, {}};
      }

      template <template <typename> typename Allocator>
      static ForwardList<T, Allocator<Node<T>>>
      create_with_allocator(Allocator<Node<T>> &&allocator) {
        return {0, nullptr, ::std::move(allocator)};
      }

      ~ForwardList() {
        while (!Stack::is_empty(*this))
          Stack::pop(*this);
      }
    };
  }

  using flist::ForwardList;
}

namespace traits {

  template <typename T, typename A>
  struct Collection::Impl<::ttl::collections::ForwardList<T, A>, void> {
  private:
    using ForwardList = ::ttl::collections::ForwardList<T, A>;

  public:
    static ::std::size_t
    size(ForwardList const &self) {
      return self.size;
    }
  };

  template <typename T, typename A>
  struct Stack::Impl<::ttl::collections::ForwardList<T, A>, void> {
  private:
    using ForwardList = ::ttl::collections::ForwardList<T, A>;

  public:
    using Item = T;

    static bool
    is_empty(ForwardList const &self) {
      return self.top == nullptr;
    }

    static void
    push(ForwardList &self, T &&item) {
      self.size += 1;
      self.top = Allocator::add(self.allocator,
                                {next : self.top, data : ::std::move(item)});
    }

    static T
    pop(ForwardList &self) {
      ASSERT(not is_empty(self));

      self.size -= 1;
      auto *ptr = self.top;
      self.top = ptr->next;
      return Allocator::remove(self.allocator, ptr).data;
    }
  };
}

#ifdef TTL_ENABLE_TEST
namespace collections {
  namespace flist {
    namespace {
      using ::ttl::test::test_stack_destruction;
      using ::ttl::test::test_stack;

      TESTCASE("test flist") {
        SECTION("destruction") {
          test_stack_destruction<ForwardList>({{}});
        }
        SECTION("stack") {
          test_stack<ForwardList>({{}});
        }
      }
    }
  }
}
#endif
