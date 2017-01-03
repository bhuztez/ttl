namespace test {
  struct Counter {
    static ::std::size_t count;
    bool valid;

    Counter()
        : valid(true) {
    }

    Counter(Counter &&o) noexcept : valid(false) {
      ::std::swap(valid, o.valid);
    }

    Counter &
    operator=(Counter &&) = delete;

    Counter(Counter const &) = delete;
    Counter &
    operator=(const Counter &) = delete;

    ~Counter() {
      if (valid)
        count++;
    }
  };

  ::std::size_t Counter::count = 0;
}
