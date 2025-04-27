# Реализация Singleton с ортогональными стратегиями на C++
---

## Немного теории

**Singleton** — шаблон, обеспечивающий единственный экземпляр класса и глобальную точку доступа к нему (логгер, конфиг и т.д.). Простейшая версия часто не учитывает многопоточность и разные способы хранения.

**Policy-based design** (ортогональные стратегии) разделяет логику на независимые блоки:

- **CreationPolicy** — создание/удаление объекта
- **ThreadingModel** — блокировки или их отсутствие

Каждый блок можно заменить или расширить без правки основного кода.

---

## Состав реализации

### 1. CreationPolicy

- `DefaultCreation<T>`: создание через `new T()` и удаление через `delete`.

```cpp
template <typename T>
struct DefaultCreation {
    static T* Create() { return new T(); }
    static void Destroy(T* p) { delete p; }
};
```

_При желании добавить: статический буфер, фабрику, пул и т.д._

### 2. ThreadingModel

- `SingleThreaded`: без блокировок, для однопоточных задач.
- `MultiThreaded`: синхронизация через `std::mutex`.

```cpp
struct SingleThreaded {
    struct Lock { Lock(...) {} };
};

struct MultiThreaded {
    struct Lock { Lock(std::mutex& m) : guard(m) {} std::lock_guard<std::mutex> guard; };
    static std::mutex& GetMutex() { static std::mutex m; return m; }
};
```

### 3. Шаблон Singleton

```cpp
template <
    typename T,
    template <typename> class CreationPolicy = DefaultCreation,
    typename ThreadingModel = SingleThreaded
>
class Singleton {
public:
    static T& Instance() {
        typename ThreadingModel::Lock lock(getMutex());
        if (!instance_) {
            instance_ = CreationPolicy<T>::Create();
            std::atexit(&Destroy);
        }
        return *instance_;
    }

private:
    static void Destroy() { CreationPolicy<T>::Destroy(instance_); instance_ = nullptr; }
    static auto& getMutex() {
        if constexpr (std::is_same_v<ThreadingModel, MultiThreaded>)
            return MultiThreaded::GetMutex();
        else
            return *reinterpret_cast<std::mutex*>(nullptr);
    }
    static T* instance_;
};

template <typename T, template <typename> class CP, typename TM>
T* Singleton<T, CP, TM>::instance_ = nullptr;
```

- При первом обращении к `Instance()` объект создаётся и регистрируется функция `Destroy()` для удаления при выходе.
- `getMutex()` возвращает настоящий мьютекс для `MultiThreaded` и «пустышку» для `SingleThreaded`.

---

## Пример использования

```cpp
class Logger {
public:
    void Log(const std::string& msg) { std::cout << msg << std::endl; }
};

using LoggerSingleton = Singleton<Logger, DefaultCreation, MultiThreaded>;

int main() {
    LoggerSingleton::Instance().Log("Начало работы");
    // ...
}
```

В этом примере `LoggerSingleton` — потокобезопасный одиночка для логирования.

---

## Как применить в проекте

1. Включить заголовок шаблона в проект.
2. Выбрать необходимые политики:
   ```cpp
   using ConfigSingleton = Singleton<Config, XMLCreation, SingleThreaded>;
   ```
3. Вызвать `ConfigSingleton::Instance()` там, где нужен Singleton.


---

