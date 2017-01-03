namespace test {

  struct AssertionFailure {
    const char *const format;
    const char *const file;
    const int line;
    const char *const expr;

    AssertionFailure(const char *format, const char *file, int line,
                     const char *expr)
        : format(format)
        , file(file)
        , line(line)
        , expr(expr) {
    }
  };

  struct Section {
    Section *prev, *next;
    const char *const file;
    const int line;
    const char *const desc;
    bool done;
    bool entered;

    Section(const char *file, int line, const char *desc)
        : prev(NULL)
        , next(NULL)
        , file(file)
        , line(line)
        , desc(desc)
        , done(false)
        , entered(false) {
    }
  };

  struct State {
    bool done;
    Section *first, *current;

    State()
        : done(false)
        , first(NULL)
        , current(NULL) {
    }
  };

  struct Cond {
    static State state;
    Section &section;

    Cond(Section &section)
        : section(section) {
    }

    ~Cond() {
      if (section.entered && !::std::uncaught_exception())
        state.current = section.prev;

      section.entered = false;

      if (section.done || state.done)
        return;

      state.done = true;
      section.done = true;
    }

    operator bool() {
      if (section.done || state.done)
        return false;

      section.prev = state.current;
      section.next = NULL;
      section.entered = true;

      if (state.current)
        state.current->next = &section;

      if (!state.first)
        state.first = &section;

      state.current = &section;

      return true;
    }
  };

  using ::std::fprintf;
  using ::std::free;

  struct TestCase {
    static TestCase *first, *last;
    TestCase *prev, *next;
    const char *file;
    const int line;
    const char *desc;
    void (*const f)();

    TestCase(const char *file, int line, const char *desc, void (*f)())
        : prev(last)
        , next(NULL)
        , file(file)
        , line(line)
        , desc(desc)
        , f(f) {
      if (!first)
        first = this;
      if (last)
        last->next = this;
      last = this;
    }

    void
    print_exception_name() {
      int status = 0;
      char *buff = ::abi::__cxa_demangle(
          ::abi::__cxa_current_exception_type()->name(), NULL, NULL, &status);
      fprintf(stderr, "Caught exception of type '%s'\n", buff);
      free(buff);
    }

    void
    print_state(const State &state) const {
      fprintf(stderr, "Testcase state:\n%s%s:%d: %s\n",
              (state.current) ? "   " : "-> ", file, line, desc);
      int count = 1;
      for (Section *p = state.first; p; p = p->next)
        fprintf(stderr, "%*s%s:%d: %s\n", (3 + 3 * count++),
                (p == state.current) ? "-> " : "", p->file, p->line, p->desc);
    }

    bool
    run() {
      bool failed = false;

      while (true) {
        Cond::state = State();

        try {
          f();
        } catch (const AssertionFailure &e) {
          failed = true;
          print_exception_name();
          fprintf(stderr, e.format, e.file, e.line, e.expr);
          print_state(Cond::state);
        } catch (const ::std::exception &e) {
          failed = true;
          print_exception_name();
          printf("  %s\n", e.what());
          print_state(Cond::state);
        } catch (...) {
          failed = true;
          print_exception_name();
          print_state(Cond::state);
        }

        if (!Cond::state.done)
          break;
      }

      return !failed;
    }
  };

  State Cond::state;
  TestCase *TestCase::first = NULL;
  TestCase *TestCase::last = NULL;

  bool
  run_tests() {
    bool failed = false;

    for (TestCase *p = TestCase::first; p; p = p->next)
      if (!(p->run()))
        failed = true;

    return not failed;
  }
}

#define _TTLTEST_NAME(s, x) __ttltest_##s##x

#define _TTLTEST_TESTCASE(x, desc)                                             \
  static void _TTLTEST_NAME(func, x)();                                        \
  static ::ttl::test::TestCase _TTLTEST_NAME(testcase, x)(                     \
      __FILE__, __LINE__, (desc), _TTLTEST_NAME(func, x));                     \
  void _TTLTEST_NAME(func, x)()

#define _TTLTEST_SECTION(x, desc)                                              \
  static ::ttl::test::Section _TTLTEST_NAME(section, x)(__FILE__, __LINE__,    \
                                                        (desc));               \
  if (auto __ttltest_cond = ::ttl::test::Cond(_TTLTEST_NAME(section, x)))

#define SECTION(desc) _TTLTEST_SECTION(__COUNTER__, (desc))
#define TESTCASE(desc) _TTLTEST_TESTCASE(__COUNTER__, (desc))

#define ASSERT(...)                                                            \
  {                                                                            \
    if (!(__VA_ARGS__))                                                        \
      throw ::ttl::test::AssertionFailure("%s:%d: ASSERTION FAILED\n  %s\n",   \
                                          __FILE__, __LINE__, #__VA_ARGS__);   \
  }

#define ASSERT_THROW(type, ...)                                                \
  {                                                                            \
    bool __ttltest_throw = false;                                              \
    try {                                                                      \
      __VA_ARGS__;                                                             \
    } catch (const type &e) {                                                  \
      __ttltest_throw = true;                                                  \
    }                                                                          \
    if (!__ttltest_throw)                                                      \
      throw ::ttl::test::AssertionFailure(                                     \
          "%s:%d: No exception caught in\n  %s\n", __FILE__, __LINE__,         \
          #__VA_ARGS__);                                                       \
  }
