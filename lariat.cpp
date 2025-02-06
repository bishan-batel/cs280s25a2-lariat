#include <iostream>
#include <iomanip>

#ifndef LARIAT_H
  #include "lariat.h"
#endif

#ifndef LARIAT_CPP
  #define LARIAT_CPP

template<typename T, int Size>
std::ostream& operator<<(std::ostream& os, const Lariat<T, Size>& list) {
  typename Lariat<T, Size>::LNode* current = list.head_;
  int index = 0;
  while (current) {
    os << "Node starting (count " << current->count << ")\n";
    for (int local_index = 0; local_index < current->count; ++local_index) {
      os << index << " -> " << current->values[local_index] << std::endl;
      ++index;
    }
    os << "-----------\n";
    current = current->next;
  }
  return os;
}

template<typename T, int Size>
Lariat<T, Size>::Lariat():
    head_{nullptr}, tail_{nullptr}, size_{0}, nodecount_{0}, asize_{Size} {}

template<typename T, int Size>
Lariat<T, Size>::~Lariat() {
  clear();
}

template<typename T, int Size>
void Lariat<T, Size>::clear() {
  while (head_) {
    LNode* next = head_->next;
    delete head_;
    head_ = next;
  }
  tail_ = nullptr;
  size_ = 0;
  nodecount_ = 0;
  asize_ = 0;
}

template<typename T, int Size>
typename Lariat<T, Size>::FindResult Lariat<T, Size>::find_element(usize i) {
  FindResult result{nullptr, 0};

  if (i >= size_) {
    return result;
  }

  usize index = 0;

  for (LNode* node = head_; node; node = node->next) {
    index += node->count;

    if (index ) {

    }
  }

  return result;
}

#endif
