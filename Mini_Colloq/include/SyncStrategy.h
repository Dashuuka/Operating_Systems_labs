#pragma once
#include <mutex>
#include <shared_mutex>

template<typename T>
class NoSync {
public:
    void lock() {}
    void unlock() {}
    void lock_shared() {}
    void unlock_shared() {}
};

template<typename T>
class MutexSync {
private:
    mutable std::shared_mutex mutex;
public:
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
    void lock_shared() { mutex.lock_shared(); }
    void unlock_shared() { mutex.unlock_shared(); }
};

template<typename T, template<typename> class SyncStrategy = NoSync>
class Synchronizable : public T {
private:
    mutable SyncStrategy<T> sync;

public:
    using T::T;

    class ScopedLock {
    private:
        SyncStrategy<T>& sync;
    public:
        explicit ScopedLock(SyncStrategy<T>& s) : sync(s) { sync.lock(); }
        ~ScopedLock() { sync.unlock(); }
    };

    class ScopedSharedLock {
    private:
        SyncStrategy<T>& sync;
    public:
        explicit ScopedSharedLock(SyncStrategy<T>& s) : sync(s) { sync.lock_shared(); }
        ~ScopedSharedLock() { sync.unlock_shared(); }
    };

    ScopedLock getLock() const { return ScopedLock(sync); }
    ScopedSharedLock getSharedLock() const { return ScopedSharedLock(sync); }
};