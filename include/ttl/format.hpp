namespace format {
  template <char...> struct STRING {};

  template <typename> struct MOD;

  template <> struct MOD<unsigned char> { using TYPE = STRING<'h', 'h', 'u'>; };

  template <> struct MOD<unsigned short> { using TYPE = STRING<'h', 'u'>; };

  template <> struct MOD<unsigned int> { using TYPE = STRING<'u'>; };

  template <> struct MOD<unsigned long> { using TYPE = STRING<'l', 'u'>; };

  template <> struct MOD<unsigned long long> {
    using TYPE = STRING<'l', 'l', 'u'>;
  };

  template <> struct MOD<signed char> { using TYPE = STRING<'h', 'h', 'd'>; };

  template <> struct MOD<signed short> { using TYPE = STRING<'h', 'd'>; };

  template <> struct MOD<signed int> { using TYPE = STRING<'d'>; };

  template <> struct MOD<signed long> { using TYPE = STRING<'l', 'd'>; };

  template <> struct MOD<signed long long> {
    using TYPE = STRING<'l', 'l', 'd'>;
  };

  template <typename S1, typename S2> struct CONCAT;

  template <char... S1, char... S2>
  struct CONCAT<STRING<S1...>, STRING<S2...>> {
    using TYPE = STRING<S1..., S2...>;
  };

  template <char...> struct CONVERT;

  template <> struct CONVERT<> { using TYPE = STRING<>; };

  template <char... S> struct CONVERT<'`', '`', S...> {
    using TYPE =
        typename CONCAT<STRING<'`'>, typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '8', 'u', S...> {
    using TYPE = typename CONCAT<typename MOD<uint8_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '8', 'd', S...> {
    using TYPE = typename CONCAT<typename MOD<int8_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '1', '6', 'u', S...> {
    using TYPE = typename CONCAT<typename MOD<uint16_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '1', '6', 'd', S...> {
    using TYPE = typename CONCAT<typename MOD<int16_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '3', '2', 'u', S...> {
    using TYPE = typename CONCAT<typename MOD<uint32_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '3', '2', 'd', S...> {
    using TYPE = typename CONCAT<typename MOD<int32_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '6', '4', 'u', S...> {
    using TYPE = typename CONCAT<typename MOD<uint64_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', '6', '4', 'd', S...> {
    using TYPE = typename CONCAT<typename MOD<int64_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', 'z', 'u', S...> {
    using TYPE = typename CONCAT<typename MOD<size_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', 'z', 'd', S...> {
    using TYPE = typename CONCAT<typename MOD<ssize_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', 'z', S...> {
    using TYPE = typename CONCAT<typename MOD<size_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char... S> struct CONVERT<'`', 't', S...> {
    using TYPE = typename CONCAT<typename MOD<ptrdiff_t>::TYPE,
                                 typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <char C, char... S> struct CONVERT<C, S...> {
    using TYPE = typename CONCAT<STRING<C>, typename CONVERT<S...>::TYPE>::TYPE;
  };

  template <typename, typename> struct MAKE_STRING;

  template <typename S, template <typename U, U...> typename T, typename U,
            U... I>
  struct MAKE_STRING<S, T<U, I...>> {
    using TYPE = typename CONVERT<S{}.s[I]...>::TYPE;
  };

  template <typename> struct STRING_LITERAL;

  template <char... S> struct STRING_LITERAL<STRING<S...>> {
    static const char s[sizeof...(S) + 1];
  };

  template <char... S>
  const char STRING_LITERAL<STRING<S...>>::s[sizeof...(S) + 1] = {S..., 0};

  template <typename S>
  using FORMAT_STRING = STRING_LITERAL<
      typename MAKE_STRING<S, ::std::make_index_sequence<sizeof(S) - 1>>::TYPE>;
}

#define FORMAT(str)                                                            \
  ({                                                                           \
    struct S {                                                                 \
      const char s[sizeof(str)] = (str);                                       \
    };                                                                         \
    ::ttl::format::FORMAT_STRING<S>::s;                                        \
  })
