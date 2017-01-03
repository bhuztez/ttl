namespace traits {

  struct Allocator {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static typename Impl<T>::Item *
    add(T &self, typename Impl<T>::Item &&item) {
      return Impl<T>::add(self, ::std::move(item));
    }

    template <typename T>
    static typename Impl<T>::Item
    remove(T &self, typename Impl<T>::Item *ptr) {
      return Impl<T>::remove(self, ptr);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        typename ::std::enable_if<
            not::std::is_copy_constructible<T>::value>::type,
        typename ::std::enable_if<not::std::is_copy_assignable<T>::value>::type,
        typename Impl<T>::Item,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::add), decltype(add<T>)>::value>::type,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::remove), decltype(remove<T>)>::value>::type>;
  };

  struct Bounded {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static ::std::size_t
    capacity(T const &self) {
      return Impl<T>::capacity(self);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<typename ::std::enable_if<::std::is_same<
        decltype(Impl<T>::capacity), decltype(capacity<T>)>::value>::type>;
  };

  struct Unbounded {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static ::std::size_t
    capacity(T const &self) {
      return Impl<T>::capacity(self);
    }

    template <typename T>
    static void
    reserve(T &self, ::std::size_t n) {
      return Impl<T>::reserve(self, n);
    }

    template <typename T>
    static void
    shrink_to_fit(T &self) {
      return Impl<T>::shrink_to_fit(self);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::capacity), decltype(capacity<T>)>::value>::type,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::reserve), decltype(reserve<T>)>::value>::type,
        typename ::std::enable_if<
            ::std::is_same<decltype(Impl<T>::shrink_to_fit),
                           decltype(shrink_to_fit<T>)>::value>::type>;
  };
}
