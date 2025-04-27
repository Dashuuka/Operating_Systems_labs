#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>

struct ISortStrategy {
    virtual ~ISortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
};

struct QuickSort : ISortStrategy {
    void sort(std::vector<int>& data) override {
        std::sort(data.begin(), data.end()); // std::sort как бы QuickSort
        std::cout << "QuickSort выполнен\n";
    }
};

struct BubbleSort : ISortStrategy {
    void sort(std::vector<int>& data) override {
        for (size_t i = 0; i < data.size(); ++i)
            for (size_t j = 0; j + 1 < data.size(); ++j)
                if (data[j] > data[j+1])
                    std::swap(data[j], data[j+1]);
        std::cout << "BubbleSort выполнен\n";
    }
};

class Sorter {
    std::unique_ptr<ISortStrategy> strategy_;
public:
    void setStrategy(std::unique_ptr<ISortStrategy> s) {
        strategy_ = std::move(s);
    }
    void sort(std::vector<int>& data) {
        strategy_->sort(data);
    }
};

void threadTask(int id, std::vector<int> data, std::unique_ptr<ISortStrategy> strat) {
    Sorter sorter;
    sorter.setStrategy(std::move(strat));
    sorter.sort(data);
    std::cout << "Поток " << id << " завершён.\n";
}

int main() {
    std::vector<int> arr1 = {5,2,9,1,5};
    std::vector<int> arr2 = {3,8,4,7,6};

    std::thread t1(threadTask, 1, arr1, std::make_unique<QuickSort>());
    std::thread t2(threadTask, 2, arr2, std::make_unique<BubbleSort>());

    t1.join();
    t2.join();
    return 0;
}
