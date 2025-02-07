#include <cassert>
#include <exception>
#include <iostream>
#include <iomanip>
#include <tuple>

#define LARIAT_CPP

#ifndef LARIAT_H
  #include "lariat.h"
#endif

template<typename T, usize Size>
std::ostream& operator<<(std::ostream& os, const Lariat<T, Size>& list) {
  typename Lariat<T, Size>::LNode* current = list.head_;
  usize index = 0;
  while (current) {
    os << "Node starting (count " << current->count << ")\n";
    for (usize local_index = 0; local_index < current->count; ++local_index) {
      os << index << " -> " << current->values[local_index] << std::endl;
      ++index;
    }
    os << "-----------\n";
    current = current->next;
  }
  return os;
}

template<typename T, usize Size>
Lariat<T, Size>::Lariat() {}

template<typename T, usize Size>
Lariat<T, Size>::Lariat(const Lariat& rhs) {
  for (const LNode* node = rhs.head_; node; node = node->next) {
    for (usize i = 0; i < node->count; i++) {
      push_back(node->values[i]);
    }
  }
}

template<typename T, usize Size>
template<typename S, usize OtherSize>
Lariat<T, Size>::Lariat(const Lariat<S, OtherSize>& rhs) {
  using Node = typename Lariat<S, OtherSize>::LNode;

  for (const Node* node = rhs.head_; node; node = node->next) {
    for (usize i = 0; i < node->count; i++) {
      push_back(static_cast<T>(node->values[i]));
    }
  }
}

template<typename T, usize Size>
Lariat<T, Size>::~Lariat() {
  // TODO:
  clear();
}

template<typename T, usize Size>
auto Lariat<T, Size>::operator=(const Lariat& rhs) -> Lariat& {
  // TODO:
  if (&rhs == this) {
    return *this;
  }

  clear();
  for (const LNode* node = rhs.head_; node; node = node->next) {
    for (usize i = 0; i < node->count; i++) {
      push_back(node->values[i]);
    }
  }

  return *this;
}

template<typename T, usize Size>
template<typename S, usize OtherSize>
auto Lariat<T, Size>::operator=(const Lariat<S, OtherSize>& rhs) -> Lariat& {
  static_assert(Size != OtherSize, "Wrong Operator (SFINAE)");

  using Node = typename Lariat<S, OtherSize>::LNode;

  clear();

  for (const Node* node = rhs.head_; node; node = node->next) {
    for (usize i = 0; i < node->count; i++) {
      push_back(static_cast<T>(node->values[i]));
    }
  }

  return *this;
}

template<typename T, usize Size>
auto Lariat<T, Size>::insert(const int index_signed, const T& value) -> void {
  // TODO:
  const usize index = static_cast<usize>(index_signed);

  if (index == size()) {
    push_back(value);
    return;
  }

  if (index > size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  if (index == 0) {
    push_front(value);
    return;
  }

  const FindResult result = find_element(index);

  LNode* node = &result.node;
  usize local_index = result.index;

  // book keeping

  if (not node->is_full()) {
    node->values[node->count++] = value;
    size_++;
    shift_up(*node, local_index);
    return;
  }

  T overflow = node->values[node->count - 1];
  shift_up(*node, local_index);
  node->values[local_index] = value;

  node->count++;
  split(*node);

  node->next->values[node->next->count - 1] = overflow;
}

template<typename T, usize Size>
auto Lariat<T, Size>::push_back(const T& value) -> void {
  if (not tail_) {
    head_ = make_node();
    tail_ = head_;
  } else if (tail_->is_full()) {
    size_++;
    tail_->count++;
    split(*tail_);
    tail_->values[tail_->count - 1] = value;
    return;
  }

  tail_->values[tail_->count] = value;
  tail_->count++;
  size_++;
}

template<typename T, usize Size>
auto Lariat<T, Size>::push_front(const T& value) -> void {
  if (not head_) {
    head_ = make_node();
    tail_ = head_;
  } else if (head_->is_full()) {
    split(*head_);
  }

  head_->values[head_->count] = value;
  head_->count++;
  size_++;
  shift_up(*head_, 0);
}

template<typename T, usize Size>
auto Lariat<T, Size>::erase(const int index_signed) -> void {
  const usize index = static_cast<usize>(index_signed);

  if (index >= size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  if (index + 1 == size()) {
    pop_back();
    return;
  }

  if (index == 0) {
    pop_front();
    return;
  }

  const auto [node, local_index] = find_element(index);

  shift_down(node, local_index);
  node.count--;
}

template<typename T, usize Size>
auto Lariat<T, Size>::pop_back() -> void {
  if (size() == 0) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }
  tail_->count--;
}

template<typename T, usize Size>
auto Lariat<T, Size>::pop_front() -> void {
  shift_down(*head_, 0);
  head_->count--;
}

template<typename T, usize Size>
auto Lariat<T, Size>::operator[](const int index_signed) -> T& {
  const auto [node, index] = find_element(static_cast<usize>(index_signed));
  return node.values[index];
}

template<typename T, usize Size>
auto Lariat<T, Size>::operator[](const int index_signed) const -> const T& {
  const auto [node, index] = find_element(static_cast<usize>(index_signed));
  return node.values[index];
}

template<typename T, usize Size>
auto Lariat<T, Size>::first() -> T& {
  if (not size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  return (*this)[0];
}

template<typename T, usize Size>
auto Lariat<T, Size>::first() const -> const T& {
  if (not size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  return (*this)[0];
}

template<typename T, usize Size>
auto Lariat<T, Size>::last() -> T& {
  if (not size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  return (*this)[size_ - 1];
}

template<typename T, usize Size>
auto Lariat<T, Size>::last() const -> const T& {
  if (not size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  return (*this)[size_ - 1];
}

template<typename T, usize Size>
auto Lariat<T, Size>::find(const T& value) const -> u32 {
  usize i = 0;

  for (LNode* node = head_; node; node = node->next) {
    for (usize j = 0; j < node->count; j++) {
      if (node->values[j] == value) {
        return static_cast<u32>(i + j);
      }
    }
    i += node->count;
  }

  return static_cast<u32>(size());
}

template<typename T, usize Size>
auto Lariat<T, Size>::size() const -> usize {
  return size_;
}

template<typename T, usize Size>
auto Lariat<T, Size>::compact() -> void {
  LNode *dest{head_}, *src{};

  if (dest) {
    src = dest->next;
  }

  usize read_idx{0}, read_end{src->count};

  while (dest and src) {
    while (not dest->is_full() and read_idx != read_end) {
      dest->values[dest->count] = src->values[read_idx];
      dest->count++;
      read_idx++;
    }

    if (read_idx == read_end) {
      src = src->next;

      if (not src) {
        break;
      }

      read_idx = 0;
      read_end = src->count;
    }
    if (dest->is_full()) {
      dest = dest->next;
    }
  }

  if (head_->count == 0) {
    clear();
    return;
  }

  while (tail_ and tail_->count == 0) {
    LNode* temp = tail_->prev;

    if (tail_->prev) {
      tail_->prev->next = tail_;
    }
    delete[] tail_;

    tail_ = temp;
  }
}

template<typename T, usize Size>
auto Lariat<T, Size>::clear() -> void {
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

template<typename T, usize Size>
auto Lariat<T, Size>::LNode::is_full() const -> bool {
  return count == Size;
}

template<typename T, usize Size>
auto Lariat<T, Size>::make_node(LNode* prev, LNode* next) const -> LNode* {
  try {
    LNode* node = new LNode();
    node->prev = prev;
    node->next = next;
    nodecount_++;
    return node;
  } catch (const std::bad_alloc&) {
    throw LariatException{LariatException::E_NO_MEMORY};
  }
}

template<typename T, usize Size>
auto Lariat<T, Size>::shift_up(LNode& node, const usize index) -> void {
  if (index >= Size) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  for (usize i = index + 1; i < node.count; i++) {
    swap(node.values[index], node.values[i]);
  }
}

template<typename T, usize Size>
auto Lariat<T, Size>::shift_down(LNode& node, usize index) -> void {
  if (index >= Size) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  for (usize i = index; i < Size - 2; i++) {
    node.values[i] = node.values[i + 1];
  }
}

template<typename T, usize Size>
auto Lariat<T, Size>::split(LNode& node) -> void {
  LNode* const next = make_node(&node, node.next);

  node.next = next;

  const usize sep_index = (node.count + 1) / 2;

  for (usize i = sep_index; i < node.count; i++) {
    next->values[i - sep_index] = node.values[i];
  }

  next->count = node.count - sep_index;
  node.count -= next->count;

  // book keeping
  if (tail_->next) {
    tail_ = tail_->next;
  }
}

template<typename T, usize Size>
auto Lariat<T, Size>::find_element(const usize i) const -> FindResult {

  if (i > size()) {
    throw LariatException{LariatException::E_BAD_INDEX};
  }

  usize index{i};

  for (LNode* node = head_; node; node = node->next) {
    if (index < node->count) {
      return {*node, index};
    }

    index -= node->count;
  }

  throw LariatException{LariatException::E_BAD_INDEX};
}

template<typename T>
auto swap(T& lhs, T& rhs) -> void {
  T tmp = std::move(lhs);
  lhs = std::move(rhs);
  rhs = std::move(tmp);
}
