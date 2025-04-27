#include <iostream>
#include <memory>
#include <vector>
#include <stack>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

struct ICommand {
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class Light {
public:
    void on()  { std::cout << "Light is ON\n"; }
    void off() { std::cout << "Light is OFF\n"; }
};

class LightOnCommand : public ICommand {
    Light& light_;
public:
    LightOnCommand(Light& l) : light_(l) {}
    void execute() override { light_.on(); }
    void undo() override    { light_.off(); }
};

class LightOffCommand : public ICommand {
    Light& light_;
public:
    LightOffCommand(Light& l) : light_(l) {}
    void execute() override { light_.off(); }
    void undo() override    { light_.on(); }
};

class CommandInvoker {
    std::stack<std::unique_ptr<ICommand>> history_;
    std::mutex mtx_;
    std::queue<std::unique_ptr<ICommand>> queue_;
    std::condition_variable cv_;
    bool stop_{false};

public:
    void enqueue(std::unique_ptr<ICommand> cmd) {
        std::lock_guard<std::mutex> lk(mtx_);
        queue_.push(std::move(cmd));
        cv_.notify_one();
    }
    void worker() {
        while (true) {
            std::unique_ptr<ICommand> cmd;
            {
                std::unique_lock<std::mutex> lk(mtx_);
                cv_.wait(lk, [&]{ return stop_ || !queue_.empty(); });
                if (stop_ && queue_.empty()) return;
                cmd = std::move(queue_.front());
                queue_.pop();
            }
            cmd->execute();
            history_.push(std::move(cmd));
        }
    }
    void stop() {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
    }
    void undoLast() {
        if (history_.empty()) return;
        auto cmd = std::move(history_.top());
        history_.pop();
        cmd->undo();
    }
};

int main() {
    Light lamp;
    CommandInvoker invoker;

    std::thread th([&]{ invoker.worker(); });

    invoker.enqueue(std::make_unique<LightOnCommand>(lamp));
    invoker.enqueue(std::make_unique<LightOffCommand>(lamp));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    invoker.undoLast();

    invoker.stop();
    th.join();
    return 0;
}
