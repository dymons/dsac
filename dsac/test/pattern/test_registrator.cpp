#include <catch2/catch.hpp>

#include <dsac/pattern/registrator/registrator.hpp>

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
struct TSingletonTraits {
  static constexpr size_t Priority = 65536;
};

namespace NPrivate {

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

void LockRecursive(std::atomic<std::thread::id>& lock) noexcept {
  const std::thread::id id = std::this_thread::get_id();

  assert(lock.load(std::memory_order_acquire) != id && "recursive singleton initialization");

  if (!MyAtomicTryLock(lock, id)) {
    do {
      std::this_thread::yield();
    } while (!MyAtomicTryAndTryLock(lock, id));
  }
}
void UnlockRecursive(std::atomic<std::thread::id>& lock) noexcept {
  assert(lock.load(std::memory_order_acquire) == std::this_thread::get_id() && "unlock from another thread?!?!");
  lock.store(std::thread::id{});
}

template <class T>
void Destroyer(void* ptr) {
  ((T*)ptr)->~T();
}

template <class T, size_t P, class... TArgs>
[[gnu::noinline]] T* SingletonBase(std::atomic<T*>& ptr, TArgs&&... args) {
  alignas(T) static char              buf[sizeof(T)];
  static std::atomic<std::thread::id> lock;

  LockRecursive(lock);

  auto ret = ptr.load();

  try {
    if (!ret) {
      ret = ::new (buf) T(std::forward<TArgs>(args)...);

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

template <class T, size_t P, class... TArgs>
T* SingletonInt(TArgs&&... args) {
  static std::atomic<T*> ptr;
  auto                   ret = ptr.load();

  if (!ret) [[unlikely]] {
    ret = SingletonBase<T, P>(ptr, std::forward<TArgs>(args)...);
  }

  return ret;
}

template <class T>
class TDefault {
public:
  template <class... TArgs>
  inline TDefault(TArgs&&... args)
    : T_(std::forward<TArgs>(args)...) {
  }

  inline const T* Get() const noexcept {
    return &T_;
  }

private:
  T T_;
};

template <class T>
struct THeapStore {
  template <class... TArgs>
  inline THeapStore(TArgs&&... args)
    : D(new T(std::forward<TArgs>(args)...)) {
  }

  inline ~THeapStore() {
    delete D;
  }

  T* D;
};
}  // namespace NPrivate

template <class T, class... TArgs>
T* Singleton(TArgs&&... args) {
  return ::NPrivate::SingletonInt<T, TSingletonTraits<T>::Priority>(std::forward<TArgs>(args)...);
}

template <class T, size_t P, class... TArgs>
T* SingletonWithPriority(TArgs&&... args) {
  return ::NPrivate::SingletonInt<T, P>(std::forward<TArgs>(args)...);
}

template <class T>
const T& Default() {
  return *(::NPrivate::SingletonInt<typename ::NPrivate::TDefault<T>, TSingletonTraits<T>::Priority>()->Get());
}

namespace dsac {
template <class TProduct, class... TArgs>
class IFactoryObjectCreator {
public:
  virtual TProduct* Create(TArgs... args) const = 0;
  virtual ~IFactoryObjectCreator() {
  }
};

template <class TProduct>
class IFactoryObjectCreator<TProduct, void> {
public:
  virtual TProduct* Create(void) const = 0;
  virtual ~IFactoryObjectCreator() {
  }
};

template <class TBaseProduct, class TDerivedProduct, class... TArgs>
class TFactoryObjectCreator : public IFactoryObjectCreator<TBaseProduct, TArgs...> {
  TDerivedProduct* Create(TArgs... args) const override {
    return new TDerivedProduct(std::forward<TArgs>(args)...);
  }
};

template <class TBaseProduct, class TDerivedProduct>
class TFactoryObjectCreator<TBaseProduct, TDerivedProduct, void> : public IFactoryObjectCreator<TBaseProduct, void> {
  TDerivedProduct* Create() const override {
    return new TDerivedProduct();
  }
};

template <class P, class K, class... TArgs>
class IObjectFactory {
public:
  typedef P TProduct;
  typedef K TKey;

public:
  template <class TDerivedProduct>
  void Register(const TKey& key, IFactoryObjectCreator<TProduct, TArgs...>* creator) {
    if (!creator) throw std::logic_error{"Please specify non-null creator for "};  //  << key

    std::unique_lock guard(CreatorsLock);
    if (!Creators.insert(typename ICreators::value_type(key, creator)).second)
      throw std::logic_error{"Product with key "};  // << key << " already registered";
  }

  template <class TDerivedProduct>
  void Register(const TKey& key) {
    Register<TDerivedProduct>(key, new TFactoryObjectCreator<TProduct, TDerivedProduct, TArgs...>);
  }

  void GetKeys(std::set<TKey>& keys) const {
    std::shared_lock guard(CreatorsLock);
    keys.clear();
    for (typename ICreators::const_iterator i = Creators.begin(), e = Creators.end(); i != e; ++i) {
      keys.insert(i->first);
    }
  }

protected:
  IFactoryObjectCreator<TProduct, TArgs...>* GetCreator(const TKey& key) const {
    std::shared_lock                   guard(CreatorsLock);
    typename ICreators::const_iterator i = Creators.find(key);
    return i == Creators.end() ? nullptr : i->second.get();
  }

  bool HasImpl(const TKey& key) const {
    std::shared_lock guard(CreatorsLock);
    return Creators.find(key) != Creators.end();
  }

private:
  typedef std::shared_ptr<IFactoryObjectCreator<TProduct, TArgs...>> ICreatorPtr;
  typedef std::map<TKey, ICreatorPtr>                                ICreators;
  ICreators                                                          Creators;
  mutable std::shared_mutex                                          CreatorsLock;
};

template <class TProduct, class TKey, class... TArgs>
class factory : public IObjectFactory<TProduct, TKey, TArgs...> {
public:
  TProduct* Create(const TKey& key, TArgs... args) const {
    IFactoryObjectCreator<TProduct, TArgs...>* creator = IObjectFactory<TProduct, TKey, TArgs...>::GetCreator(key);
    return creator == nullptr ? nullptr : creator->Create(std::forward<TArgs>(args)...);
  }

  static bool Has(const TKey& key) {
    return Singleton<factory<TProduct, TKey, TArgs...>>()->HasImpl(key);
  }

  static std::unique_ptr<TProduct> construct(const TKey& key, TArgs... args) {
    return std::unique_ptr<TProduct>{
        Singleton<factory<TProduct, TKey, TArgs...>>()->Create(key, std::forward<TArgs>(args)...)};
  }

  static void GetRegisteredKeys(std::set<TKey>& keys) {
    return Singleton<factory<TProduct, TKey, TArgs...>>()->GetKeys(keys);
  }

  static std::set<TKey> GetRegisteredKeys() {
    std::set<TKey> keys;
    Singleton<factory<TProduct, TKey, TArgs...>>()->GetKeys(keys);
    return keys;
  }

  template <class TDerivedProduct>
  static std::set<TKey> GetRegisteredKeys() {
    std::set<TKey> registeredKeys(GetRegisteredKeys());
    std::set<TKey> fileredKeys;
    std::copy_if(
        registeredKeys.begin(),
        registeredKeys.end(),
        std::inserter(fileredKeys, fileredKeys.end()),
        [](const TKey& key) {
          std::unique_ptr<TProduct> objectHolder(construct(key));
          return !!dynamic_cast<const TDerivedProduct*>(objectHolder.Get());
        });
    return fileredKeys;
  }

  template <class Product>
  class registractor {
  public:
    explicit registractor(const TKey& key) {
      Singleton<factory<TProduct, TKey, TArgs...>>()->template Register<Product>(key);
    }

    registractor()
      : registractor(Product::GetTypeName()) {
    }
  };
};

}  // namespace dsac

class executor_base {
public:
  using factory = dsac::factory<executor_base, std::string>;

  executor_base()                                = default;
  executor_base(const executor_base&)            = default;
  executor_base(executor_base&&)                 = default;
  executor_base& operator=(const executor_base&) = default;
  executor_base& operator=(executor_base&&)      = default;
  virtual ~executor_base()                       = default;
};

class static_thread_pool final : public executor_base {
public:
  static const factory::registractor<static_thread_pool> kRegistractor;
};

const executor_base::factory::registractor<static_thread_pool> static_thread_pool::kRegistractor("static_thread_pool");

TEST_CASE("", "") {
  const std::string static_thread_pool_module = "static_thread_pool";
  auto              static_thread_pool        = executor_base::factory::construct(static_thread_pool_module);
}