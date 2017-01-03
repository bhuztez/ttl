namespace traits {
  // template<typename ...Args>
  // using VALID = void;

  // template<typename, typename>
  // struct SAME_TYPE;

  // template<typename T>
  // struct SAME_TYPE<T,T> {
  // };

  // template<typename T>
  // struct TYPE {
  //   template<typename ...>
  //   using REQUIRE = T;
  // };

  template <typename T, typename Trait>
  using IMPLEMENTS = decltype(Trait::template REQUIRE<T>());
}
