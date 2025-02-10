////////////////////////////////////////////////////////////////////////////////
#ifndef LARIAT_H
#define LARIAT_H
////////////////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <string>  // error strings
#include <utility> // error strings
#include <cstring> // memcpy
                   //

/**
 * @brief 32 Bit Floating Point Number
 */
using f32 = float;

/**
 * @brief 64 Bit Floating Point Number
 */
using f64 = long double;

/**
 * @brief Fix Sized Unsigned 8 Bit Integer (cannot be negative)
 */
using u8 = std::uint8_t;

/**
 * @brief Fix Sized Unsigned 16 Bit Integer (cannot be negative)
 */
using u16 = std::uint16_t;

/**
 * @brief Fix Sized Unsigned 32 Bit Integer (cannot be negative)
 */
using u32 = std::uint32_t;

/**
 * @brief Fix Sized Unsigned 64 Bit Integer (cannot be negative)
 */
using u64 = unsigned long int;

/**
 * @brief Biggest Unsigned Integer type that the current platform can use
 * (cannot be negative)
 */
using umax = std::uintmax_t;

/**
 * @brief Unsigned Integer for when referring to any form of memory size or
 * offset (eg. an array length or index)
 */
using usize = std::size_t;
/**
 * @brief Unsigned Integer Pointer typically used for pointer arithmetic
 */
using uptr = std::uintptr_t;

/**
 * @brief Signed 8 bit Integer
 */
using i8 = std::int8_t;

/**
 * @brief Signed 16 bit Integer
 */
using i16 = std::int16_t;

/**
 * @brief Signed 32 bit Integer
 */
using i32 = std::int32_t;

/**
 * @brief Signed 64 bit Integer
 */
using i64 = std::int64_t;

/**
 * @brief Integer pointer typically used for pointer arithmetic
 */
using iptr = std::intptr_t;

class LariatException : public std::exception {
private:

  int m_ErrCode;
  std::string m_Description;

public:

  LariatException(int ErrCode, const std::string& Description):
      m_ErrCode(ErrCode), m_Description(Description) {}

  LariatException(int ErrCode): m_ErrCode(ErrCode), m_Description{} {
    switch (ErrCode) {
      case E_NO_MEMORY: m_Description = "E_NO_MEMORY"; break;
      case E_BAD_INDEX: m_Description = "Subscript is out of range"; break;
      case E_DATA_ERROR: m_Description = "Data Error"; break;
      default: break;
    }
  }

  virtual int code(void) const { return m_ErrCode; }

  virtual const char* what(void) const throw() { return m_Description.c_str(); }

  virtual ~LariatException() throw() {}

  enum LARIAT_EXCEPTION {
    E_NO_MEMORY,
    E_BAD_INDEX,
    E_DATA_ERROR
  };
};

// forward declaration for 1-1 operator<<
template<typename T, usize Size>
class Lariat;

template<typename T, usize Size>
std::ostream& operator<<(std::ostream& os, const Lariat<T, Size>& rhs);

/**
 * @brief Rope Data structure
 */
template<typename T, usize Size>
class Lariat {
public:

  template<typename S, usize OtherSize>
  friend class Lariat;

  /**
   * @brief Default constructor
   */
  Lariat();

  /**
   * @brief Copy constructor
   */
  Lariat(const Lariat& rhs);

  /**
   * @brief Copy constructor from another lariat type
   *
   * @tparam S Other type that can be casted to T
   * @param rhs other instance of a lariat type
   */
  template<typename S, usize OtherSize>
  Lariat(const Lariat<S, OtherSize>& rhs);

  Lariat(Lariat&&) = delete;

  /**
   * @brief Destructor
   */
  ~Lariat();

  /**
   * @brief Copy assignment
   */
  auto operator=(const Lariat& rhs) -> Lariat&;

  /**
   * @brief Copy assignment
   */
  template<typename S, usize OtherSize>
  auto operator=(const Lariat<S, OtherSize>& rhs) -> Lariat&;

  auto operator=(Lariat&&) -> Lariat& = delete;

  /**
   * @brief Insert a value into the given index
   *
   * @param index_signed signed index, this should be unsigned but I can't
   * change the interface
   * @param value Value to insert
   */
  auto insert(int index_signed, const T& value) -> void;

  /**
   * @brief Pushes a value to the end of the list
   */
  auto push_back(const T& value) -> void;

  /**
   * @brief Pushes a value to the beginning of the list
   */
  auto push_front(const T& value) -> void;

  /**
   * @brief Erases value at the given index
   *
   * @param index_signed signed index, this should be unsigned but I can't
   * change the interface
   */
  auto erase(int index_signed) -> void;

  /**
   * @brief Removes a value from the end of the list
   */
  auto pop_back() -> void;

  /**
   * @brief Removes a value from the beginning of the list
   */
  auto pop_front() -> void;

  /**
   * @brief Gives the value at the given index
   *
   * @param index_signed signed index, this should be unsigned but I can't
   * change the interface
   */
  [[nodiscard]] auto operator[](int index_signed) -> T&;

  /**
   * @brief Gives the value at the given index
   *
   * @param index_signed signed index, this should be unsigned but I can't
   * change the interface
   */
  [[nodiscard]] auto operator[](int index_signed) const -> const T&;

  /**
   * @brief Gets the first element of the list, throws if empty
   */
  [[nodiscard]] auto first() -> T&;

  /**
   * @brief Gets the first element of the list, throws if empty
   */
  [[nodiscard]] auto first() const -> const T&;

  /**
   * @brief Gets the last element of the list, throws if empty
   */
  [[nodiscard]] auto last() -> T&;

  /**
   * @brief Gets the last element of the list, throws if empty
   */
  [[nodiscard]] auto last() const -> const T&;

  // returns index, size (one past last) if not found
  [[nodiscard]] auto find(const T& value) const -> u32;

  friend std::ostream& operator<< <T, Size>(
    std::ostream& os,
    const Lariat<T, Size>& list
  );

  /**
   * @brief Returns the size of this list
   */
  [[nodiscard]] auto size(void) const -> usize;

  /**
   * @brief Clears this list
   */
  auto clear() -> void; // make it empty

  /**
   * @Brief Pushes data in front reusing empty positions and delete remaining
   * nodes
   */
  auto compact() -> void;

private:

  /**
   * @brief Individual node in the structure
   */
  struct LNode {
    LNode* next = nullptr;
    LNode* prev = nullptr;

    // number of items currently in the node
    usize count = 0;

    auto is_full() const -> bool;

    T values[Size];
  };

  /**
   * @brief Result given with find_element
   */
  struct FindResult {
    LNode& node{nullptr};
    usize index{0};
  };

  /**
   * @brief Factory method for LNode
   */
  [[nodiscard]] auto make_node(LNode* prev = nullptr, LNode* next = nullptr)
    const -> LNode*;

  /**
   * @brief Shifts all elemenets in the node up , starting at the given index
   */
  auto shift_up(LNode& node, usize index) -> void;

  /**
   * @brief Shifts all elements in the node down , starting at the given index
   */
  auto shift_down(LNode& node, usize index) -> void;

  /**
   * @brief Splits node into 2 roughly equal sized nodes
   */
  auto split(LNode& node) -> void;

  /**
   * @brief Locates the element with the given global index
   *
   * @param i Global index into this list
   */
  [[nodiscard]] auto find_element(usize i) const -> FindResult;

  /**
   * @brief Points to the first node
   */
  LNode* head_{nullptr};

  /**
   * @brief Points to the last node
   */
  LNode* tail_{nullptr};

  /**
   * @brief The number of items (not nodes) in the list
   */
  usize size_{0};

  /**
   * @brief // the number of nodes in the list
   */
  mutable usize nodecount_{0};

  /**
   * @brief The size of the array within the nodes
   */
  usize asize_{0};
};

/**
 * @brief Generic swap function
 */
template<typename T>
auto swap(T& lhs, T& rhs) -> void;

#ifndef LARIAT_CPP
  #include "lariat.cpp"
#endif

#endif // LARIAT_H
