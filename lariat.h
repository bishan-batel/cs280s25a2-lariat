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
template<typename T, int Size>
class Lariat;

template<typename T, int Size>
std::ostream& operator<<(std::ostream& os, const Lariat<T, Size>& rhs);

template<typename T, int Size>
class Lariat {
public:

  Lariat();                  // default constructor
  Lariat(const Lariat& rhs); // copy constructor
  ~Lariat();                 // destructor
  // more ctor(s) and assignment(s)

  void insert(int index, const T& value);
  void push_back(const T& value);
  void push_front(const T& value);

  void erase(int index);
  void pop_back();
  void pop_front();

  T& operator[](int index);

  const T& operator[](int index) const;

  T& first();

  const T& first() const;

  T& last();

  const T& last() const;

  // returns index, size (one past last) if not found
  unsigned find(const T& value) const;

  friend std::ostream& operator<< <T, Size>(
    std::ostream& os,
    const Lariat<T, Size>& list
  );

  // and some more
  size_t size(void) const; // total number of items (not nodes)
  void clear();            // make it empty

  // push data in front reusing empty positions and delete remaining nodes
  void compact();

private:

  struct LNode { // DO NOT modify provided code
    LNode* next = nullptr;
    LNode* prev = nullptr;

    // number of items currently in the node
    int count = 0;

    T values[Size];
  };

  struct FindResult {
    LNode* node;
    usize index{0};
  };

  FindResult find_element(usize i);

  // DO NOT modify provided code
  LNode* head_;           // points to the first node
  LNode* tail_;           // points to the last node
  int size_;              // the number of items (not nodes) in the list
  mutable int nodecount_; // the number of nodes in the list
  int asize_;             // the size of the array within the nodes
};

#ifndef LARIAT_CPP
  #include "lariat.cpp"
#endif

#endif // LARIAT_H
