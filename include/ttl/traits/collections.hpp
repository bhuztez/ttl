namespace traits {
  struct CapacityPolicy {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static ::std::size_t
    initial(::std::size_t capacity) {
      return Impl<T>::initial(capacity);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<typename ::std::enable_if<::std::is_same<
        decltype(Impl<T>::initial), decltype(initial<T>)>::value>::type>;
  };

  struct ResizingPolicy {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static ::std::size_t
    grow(::std::size_t capacity) {
      return Impl<T>::grow(capacity);
    }

    template <typename T>
    static ::std::size_t
    shrink(::std::size_t size, ::std::size_t capacity) {
      return Impl<T>::shrink(size, capacity);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        IMPLEMENTS<T, CapacityPolicy>,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::grow), decltype(grow<T>)>::value>::type,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::shrink), decltype(shrink<T>)>::value>::type>;
  };

  struct Collection {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static ::std::size_t
    size(T const &self) {
      return Impl<T>::size(self);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<typename ::std::enable_if<::std::is_same<
        decltype(Impl<T>::size), decltype(size<T>)>::value>::type>;
  };

  struct List {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static typename T::Item const &
    get(T const &self, ::std::size_t index) {
      return Impl<T>::get(self, index);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        IMPLEMENTS<T, Collection>, typename Impl<T>::Item,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::get), decltype(get<T>)>::value>::type>;
  };

  struct ListMut {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static typename T::Item &
    get(T &self, ::std::size_t index) {
      return Impl<T>::get(self, index);
    }

    template <typename T>
    static void
    set(T &self, ::std::size_t index, typename T::Item &&item) {
      Impl<T>::get(self, index) = ::std::move(item);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        IMPLEMENTS<T, List>, typename Impl<T>::Item,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::get), decltype(get<T>)>::value>::type>;
  };

  struct Stack {
    template <typename T, typename = void> struct Impl;

    template <typename T>
    static bool
    is_empty(T const &self) {
      return Impl<T>::is_empty(self);
    }

    template <typename T>
    static void
    push(T &self, typename Impl<T>::Item &&item) {
      return Impl<T>::push(self, ::std::move(item));
    }

    template <typename T>
    static typename Impl<T>::Item
    pop(T &self) {
      return Impl<T>::pop(self);
    }

    template <typename T>
    constexpr static auto
    REQUIRE() -> ::std::void_t<
        IMPLEMENTS<T, Collection>, typename Impl<T>::Item,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::is_empty), decltype(is_empty<T>)>::value>::type,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::push), decltype(push<T>)>::value>::type,
        typename ::std::enable_if<::std::is_same<
            decltype(Impl<T>::pop), decltype(pop<T>)>::value>::type>;
  };
}
