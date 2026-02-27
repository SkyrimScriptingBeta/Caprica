#pragma once
#include <cstddef>
#include <stdexcept>

namespace caprica {

template <typename T>
struct IntrusiveLinkedList final {
  IntrusiveLinkedList() = default;
  IntrusiveLinkedList(const IntrusiveLinkedList&) = delete;
  IntrusiveLinkedList(IntrusiveLinkedList&&) = default;
  IntrusiveLinkedList& operator=(const IntrusiveLinkedList&) = delete;
  IntrusiveLinkedList& operator=(IntrusiveLinkedList&&) = default;
  ~IntrusiveLinkedList() = default;

  T* back() { return mBack; }
  const T* back() const { return mBack; }

  T* front() { return mFront; }
  const T* front() const { return mFront; }

  void push_back(T* val) {
    val->next = nullptr;
    mSize++;
    if (mFront == nullptr) {
      mFront = mBack = val;
    } else {
      mBack->next = val;
      mBack = val;
    }
  }

  void push_front(T* val) {
    mSize++;
    if (mFront == nullptr) {
      mFront = mBack = val;
    } else {
      val->next = mFront;
      mFront = val;
    }
  }

  T* pop_front() {
    mSize--;
    auto ret = mFront;
    mFront = ret->next;
    if (mFront == nullptr)
      mBack = nullptr;
    return ret;
  }

  size_t size() const { return mSize; }

  // Accessor for T::next — public so cross-specialization lockstep
  // iteration (IntrusiveLinkedList<A> iterating with IntrusiveLinkedList<B>)
  // can take the address of the other specialization's nextOf.
  static T* nextOf(T* node) { return node->next; }
  static const T* nextOf(const T* node) { return node->next; }

private:
  size_t mSize { 0 };
  T* mFront { nullptr };
  T* mBack { nullptr };

  struct ConstIterator final {
    using NextFn = const T* (*)(const T*);
    size_t index { 0 };

    ConstIterator& operator++() {
      if (cur == nullptr)
        return *this;
      index++;
      cur = nextFn(cur);
      return *this;
    }

    const T*& operator*() { return cur; }
    const T*& operator*() const { return cur; }
    const T*& operator->() { return cur; }
    const T*& operator->() const { return cur; }

    bool operator==(const ConstIterator& other) const { return cur == other.cur; }

    bool operator!=(const ConstIterator& other) const { return !(*this == other); }

  private:
    friend IntrusiveLinkedList;
    const T* cur { nullptr };
    NextFn nextFn { nullptr };

    ConstIterator() = default;
    ConstIterator(const T* front, NextFn fn) : cur(front), nextFn(fn) { }
  };

  struct Iterator final {
    using NextFn = T* (*)(T*);
    size_t index { 0 };

    Iterator& operator++() {
      if (cur == nullptr)
        return *this;
      index++;
      cur = nextFn(cur);
      return *this;
    }

    T*& operator*() { return cur; }
    const T*& operator*() const { return cur; }
    T*& operator->() { return cur; }
    const T*& operator->() const { return cur; }

    bool operator==(const Iterator& other) const { return cur == other.cur; }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

  private:
    friend IntrusiveLinkedList;
    T* cur { nullptr };
    NextFn nextFn { nullptr };

    Iterator() = default;
    Iterator(T* front, NextFn fn) : cur(front), nextFn(fn) { }
  };

  template <typename T2>
  friend struct ConstLockstepIteratorWrapper;
  template <typename T2>
  friend struct LockstepIteratorWrapper;

public:
  template <typename T2>
  struct LockstepIterator final {
    using SelfNextFn = T* (*)(T*);
    using OtherNextFn = T2* (*)(T2*);
    size_t index { 0 };

    LockstepIterator& operator++() {
      if (cur.self == nullptr)
        return *this;
      index++;
      cur.prevSelf = cur.self;
      cur.prevOther = cur.other;
      cur.self = selfNextFn(cur.self);
      cur.other = otherNextFn(cur.other);
      return *this;
    }

    auto& operator*() { return cur; }
    const auto& operator*() const { return cur; }
    auto& operator->() { return cur; }
    const auto& operator->() const { return cur; }

    bool operator==(const LockstepIterator& other) const {
      return cur.self == other.cur.self && cur.other == other.cur.other;
    }

    bool operator!=(const LockstepIterator& other) const { return !(*this == other); }

    LockstepIterator() = default;
    LockstepIterator(T* selfFront, T2* otherFront, SelfNextFn sFn, OtherNextFn oFn)
        : selfNextFn(sFn), otherNextFn(oFn) {
      cur.self = selfFront;
      cur.other = otherFront;
    }

    struct {
      T* self { nullptr };
      T2* other { nullptr };
      T* prevSelf { nullptr };
      T2* prevOther { nullptr };
    } cur {};

  private:
    SelfNextFn selfNextFn { nullptr };
    OtherNextFn otherNextFn { nullptr };
  };

private:
  template <typename T2>
  struct LockstepIteratorWrapper final {
    LockstepIterator<T2> begin() {
      if (!self.size())
        return LockstepIterator<T2>();
      return LockstepIterator<T2>(self.mFront, other.mFront,
                                  &IntrusiveLinkedList::nextOf,
                                  &IntrusiveLinkedList<T2>::nextOf);
    }

    LockstepIterator<T2> end() { return LockstepIterator<T2>(); }

  private:
    friend IntrusiveLinkedList;
    IntrusiveLinkedList& self;
    IntrusiveLinkedList<T2>& other;

    LockstepIteratorWrapper(IntrusiveLinkedList& pSelf, IntrusiveLinkedList<T2>& pOther)
        : self(pSelf), other(pOther) { }
  };

  template <typename T2>
  struct ConstLockstepIteratorWrapper final {
    LockstepIterator<const T2> begin() {
      if (!self.size())
        return LockstepIterator<const T2>();
      return LockstepIterator<const T2>(self.front(), other.front(),
                                        &IntrusiveLinkedList::nextOf,
                                        &IntrusiveLinkedList<T2>::nextOf);
    }

    LockstepIterator<const T2> end() { return LockstepIterator<const T2>(); }

  private:
    friend IntrusiveLinkedList;
    IntrusiveLinkedList& self;
    const IntrusiveLinkedList<T2>& other;

    ConstLockstepIteratorWrapper(IntrusiveLinkedList& pSelf, const IntrusiveLinkedList<T2>& pOther)
        : self(pSelf), other(pOther) { }
  };

  struct InsertableIterator final {
    using NextFn = T* (*)(T*);

    InsertableIterator& operator++() {
      if (cur == nullptr)
        return *this;
      prev = cur;
      cur = nextFn(cur);
      return *this;
    }

    auto& operator*() { return cur; }
    const auto& operator*() const { return cur; }
    auto& operator->() { return cur; }
    const auto& operator->() const { return cur; }

    bool operator==(const InsertableIterator& other) const { return cur == other.cur; }

    bool operator!=(const InsertableIterator& other) const { return !(*this == other); }

  private:
    friend IntrusiveLinkedList;
    T* cur { nullptr };
    T* prev { nullptr };
    NextFn nextFn { nullptr };

    InsertableIterator() = default;
    InsertableIterator(T* curFront, NextFn fn) : cur(curFront), nextFn(fn) { }
  };

public:
  ConstIterator begin() const {
    if (!mSize)
      return ConstIterator();
    return ConstIterator(mFront, &IntrusiveLinkedList::nextOf);
  }

  ConstIterator end() const { return ConstIterator(); }

  Iterator begin() {
    if (!mSize)
      return Iterator();
    return Iterator(mFront, &IntrusiveLinkedList::nextOf);
  }

  Iterator end() { return Iterator(); }

  template <typename T2>
  LockstepIteratorWrapper<T2> lockstepIterate(IntrusiveLinkedList<T2>& other) {
    return LockstepIteratorWrapper<T2>(*this, other);
  }

  template <typename T2>
  ConstLockstepIteratorWrapper<T2> lockstepIterate(const IntrusiveLinkedList<T2>& other) {
    return ConstLockstepIteratorWrapper<T2>(*this, other);
  }

  InsertableIterator beginInsertable() {
    if (!mSize)
      return InsertableIterator();
    return InsertableIterator(mFront, &IntrusiveLinkedList::nextOf);
  }

  InsertableIterator endInsertable() { return InsertableIterator(); }

  void replace(InsertableIterator& loc, T&& val) {
    if (!loc.cur)
      throw std::runtime_error("Attempted to replace a non-existent value!");
    val.next = loc.cur->next;
    if (!loc.prev) {
      *loc.cur = val;
    } else {
      auto* prev = loc.prev;
      *loc.cur = val;
      prev->next = loc.cur;
    }
  }

  void insertBefore(InsertableIterator& loc, T* val) {
    if (!loc.cur) {
      push_back(val);
    } else if (!loc.prev) {
      mSize++;
      val->next = mFront;
      mFront = val;
    } else {
      mSize++;
      val->next = loc.prev->next;
      loc.prev->next = val;
    }
    loc.prev = val;
  }
};

}
