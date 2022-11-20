#pragma once

#include <fmt/format.h>

#include <cassert>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

#include <atomic>
#include <new>
#include <utility>

template <class T>
struct TsingletonTraits {
  static constexpr size_t Priority = 65536;
};

namespace dsac {

static inline bool MyAtomicTryLock(std::atomic<std::thread::id>& a, std::thread::id v) noexcept {
  std::thread::id zero{};
  return a.compare_exchange_strong(zero, v);
}

static inline bool MyAtomicTryAndTryLock(std::atomic<std::thread::id>& a, std::thread::id v) noexcept {
  return a.load(std::memory_order_acquire) == std::thread::id{} && MyAtomicTryLock(a, v);
}

using TAtExitFunc = void (*)(void*);

class TAtExit {
  struct TFunc {
    TAtExitFunc Func;
    void*       Ctx;
    size_t      Priority;
    size_t      Number;
  };

  struct TCmp {
    inline bool operator()(const TFunc* l, const TFunc* r) const noexcept {
      return std::tie(l->Priority, l->Number) < std::tie(r->Priority, r->Number);
    }
  };

public:
  inline TAtExit() noexcept
    : FinishStarted_(false) {
  }

  inline void Finish() noexcept {
    FinishStarted_.store(true);

    std::unique_lock guard(Lock_);

    while (!Items_.empty()) {
      auto c = Items_.top();

      assert(c);

      Items_.pop();

      {
        guard.unlock();

        try {
          c->Func(c->Ctx);
        } catch (...) {
          // ¯\_(ツ)_/¯
        }

        guard.lock();
      }
    }
  }

  inline void Register(TAtExitFunc func, void* ctx, size_t priority) {
    std::lock_guard guard(Lock_);
    {
      Store_.push_back({func, ctx, priority, Store_.size()});
      Items_.push(&Store_.back());
    }
  }

  inline bool FinishStarted() const {
    return FinishStarted_.load();
  }

private:
  std::mutex                                             Lock_;
  std::atomic<bool>                                      FinishStarted_;
  std::deque<TFunc>                                      Store_;
  std::priority_queue<TFunc*, std::vector<TFunc*>, TCmp> Items_;
};

static std::mutex            atExitLock;
static std::atomic<TAtExit*> atExitPtr = nullptr;
alignas(TAtExit) static char atExitMem[sizeof(TAtExit)];

static void OnExit() {
  if (TAtExit* const atExit = atExitPtr.load()) {
    atExit->Finish();
    atExit->~TAtExit();
    atExitPtr.store(nullptr);
  }
}

static inline TAtExit* Instance() {
  if (TAtExit* const atExit = atExitPtr.load(std::memory_order_acquire)) {
    return atExit;
  }
  std::lock_guard guard(atExitLock);
  {
    if (TAtExit* const atExit = atExitPtr.load()) {
      return atExit;
    }
    atexit(OnExit);
    TAtExit* const atExit = new (atExitMem) TAtExit;
    atExitPtr.store(atExit, std::memory_order_release);
    return atExit;
  }
}

static void AtExit(TAtExitFunc func, void* ctx, size_t priority) {
  Instance()->Register(func, ctx, priority);
}

void FillWithTrash(void* ptr, size_t len);

static void LockRecursive(std::atomic<std::thread::id>& lock) noexcept {
  const std::thread::id id = std::this_thread::get_id();

  assert(lock.load(std::memory_order_acquire) != id && "recursive singleton initialization");

  if (!MyAtomicTryLock(lock, id)) {
    do {
      std::this_thread::yield();
    } while (!MyAtomicTryAndTryLock(lock, id));
  }
}
static void UnlockRecursive(std::atomic<std::thread::id>& lock) noexcept {
  assert(lock.load(std::memory_order_acquire) == std::this_thread::get_id() && "unlock from another thread?!?!");
  lock.store(std::thread::id{});
}

template <class T>
void Destroyer(void* ptr) {
  ((T*)ptr)->~T();
}

template <class T, size_t P, class... Args>
[[gnu::noinline]] T* singletonBase(std::atomic<T*>& ptr, Args&&... args) {
  alignas(T) static char              buf[sizeof(T)];
  static std::atomic<std::thread::id> lock;

  LockRecursive(lock);

  auto ret = ptr.load();

  try {
    if (!ret) {
      ret = ::new (buf) T(std::forward<Args>(args)...);

      try {
        AtExit(Destroyer<T>, ret, P);
      } catch (...) {
        Destroyer<T>(ret);

        throw;
      }

      ptr.store(ret);
    }
  } catch (...) {
    UnlockRecursive(lock);

    throw;
  }

  UnlockRecursive(lock);

  return ret;
}

template <class T, size_t P, class... Args>
T* singletonInt(Args&&... args) {
  static std::atomic<T*> ptr;
  auto                   ret = ptr.load();

  if (!ret) [[unlikely]] {
    ret = singletonBase<T, P>(ptr, std::forward<Args>(args)...);
  }

  return ret;
}

template <class T, class... Args>
T* singleton(Args&&... args) {
  return singletonInt<T, TsingletonTraits<T>::Priority>(std::forward<Args>(args)...);
}

template <class T, size_t P, class... Args>
T* singletonWithPriority(Args&&... args) {
  return singletonInt<T, P>(std::forward<Args>(args)...);
}

}  // namespace dsac
