#include <iostream>
#include <thread>
#include <vector>

class DataProcessor {
public:
    void process() {           
        loadData();
        preProcess();
        processData();         
        postProcess();
        saveResults();
    }
    virtual ~DataProcessor() = default;

protected:
    virtual void loadData()    { std::cout << "Loading data...\n"; }
    virtual void preProcess()  { std::cout << "Pre-processing...\n"; }
    virtual void processData() {
        std::cout << "Processing in parallel:\n";
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([i](){
                std::cout << "  Thread " << i << " working\n";
            });
        }
        for (auto& t : threads) t.join();
    }
    virtual void postProcess() { std::cout << "Post-processing...\n"; }
    virtual void saveResults() { std::cout << "Saving results...\n"; }
};

class CSVProcessor : public DataProcessor {
protected:
    void loadData() override   { std::cout << "Reading CSV file...\n"; }
    void saveResults() override{ std::cout << "Writing CSV output...\n"; }
};

int main() {
    CSVProcessor proc;
    proc.process();
    return 0;
}
