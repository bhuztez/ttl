namespace storage {

  template <typename T, typename = void> struct Chunk;

  template <typename T>
  struct Chunk<
      T, typename ::std::enable_if<::std::is_nothrow_move_constructible<
             T>::value && ::std::is_nothrow_destructible<T>::value>::type> {
    ::std::size_t capacity;
    T *data;

    Chunk(Chunk const &) = delete;
    Chunk &
    operator=(Chunk const &) = delete;

    Chunk(Chunk &&o) noexcept : capacity(0), data(nullptr) {
      ::std::swap(capacity, o.capacity);
      ::std::swap(data, o.data);
    }

    Chunk(::std::size_t capacity)
        : capacity(capacity)
        , data(nullptr) {
      ::std::size_t size = sizeof(T) * capacity;
      data = static_cast<T *>(::std::malloc(size));
    }

    void
    resize(::std::size_t new_capacity) {
      ::std::size_t size = sizeof(T) * new_capacity;
      data = static_cast<T *>(::std::realloc(data, size));
      capacity = new_capacity;
    }

    T *
    get_ptr(::std::size_t index) {
      ASSERT(index < capacity);
      return data + index;
    }

    T const *
    get_ptr(::std::size_t index) const {
      ASSERT(index < capacity);
      return data + index;
    }

    void
    write(::std::size_t index, T &&item) {
      T *ptr = get_ptr(index);
      new (ptr) T(::std::move(item));
    }

    T
    read(::std::size_t index) {
      T *ptr = get_ptr(index);
      T item(::std::move(*ptr));
      ptr->~T();
      return item;
    }

    T &
    get(::std::size_t index) {
      return *get_ptr(index);
    }

    T const &
    get(::std::size_t index) const {
      return *get_ptr(index);
    }

    ~Chunk() {
      if (data != nullptr)
        ::std::free(data);
    }
  };
}
