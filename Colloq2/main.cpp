#include <iostream>
#include <mutex>
#include <memory>

// Creation policy: default uses new/delete
template <typename T>
struct DefaultCreation {
    static T* Create() {
        return new T();
    }
    static void Destroy(T* p) {
        delete p;
    }
};

// Threading policy: no locking (single-threaded)
struct SingleThreaded {
    struct Lock {
        Lock(...) {}
    };
};

// Threading policy: mutex-based locking (multi-threaded)
struct MultiThreaded {
    struct Lock {
        Lock(std::mutex& m) : guard(m) {}
        std::lock_guard<std::mutex> guard;
    };
    static std::mutex& GetMutex() {
        static std::mutex mtx;
        return mtx;
    }
};

// The orthogonal-strategy Singleton holder
template <
    typename T,
    template <typename> class CreationPolicy = DefaultCreation,
    typename ThreadingModel = SingleThreaded
>
class Singleton {
public:
    // Accessor
    static T& Instance() {
        // lock if needed
        typename ThreadingModel::Lock lock(GetMutexIfAny());

        if (!ptr_) {
            ptr_ = CreationPolicy<T>::Create();
            std::atexit(&Destroy);  // optional cleanup at exit
        }
        return *ptr_;
    }

private:
    // destroy instance at program end
    static void Destroy() {
        CreationPolicy<T>::Destroy(ptr_);
        ptr_ = nullptr;
    }

    // helper: return mutex reference or dummy
    static auto& GetMutexIfAny() {
        if constexpr (std::is_same_v<ThreadingModel, MultiThreaded>) {
            return ThreadingModel::GetMutex();
        } else {
            return *(static_cast<std::mutex*>(nullptr)); // won't be used
        }
    }

    static T* ptr_;
};

// static member definition
template <typename T, template <typename> class CP, typename TM>
T* Singleton<T, CP, TM>::ptr_ = nullptr;

// === Example usage ===
class Logger {
public:
    void Log(const std::string& msg) {
        std::cout << "[LOG] " << msg << "\n";
    }
};

// Choose a thread-safe singleton for Logger:
using LoggerSingleton = Singleton<Logger, DefaultCreation, MultiThreaded>;

int main() {
    // both calls return the same instance, safely in multithreaded context
    LoggerSingleton::Instance().Log("Hello, Singleton!");
    LoggerSingleton::Instance().Log("Another message");
    return 0;
}
