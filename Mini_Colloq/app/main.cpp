#include <iostream>
#include <thread>
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include "ConcreteVisitors.h"
#include "SyncStrategy.h"

int main() {
    // Создаем файловую систему
    auto disk = std::make_shared<Disk>("C:", 500000000);

    auto root = std::make_shared<Folder>("root");
    auto docs = std::make_shared<Folder>("Documents");
    auto programs = std::make_shared<Folder>("Programs");

    docs->addChild(std::make_shared<File>("report.docx", 2048, FileType::BINARY));
    docs->addChild(std::make_shared<File>("notes.txt", 512, FileType::TEXT));

    programs->addChild(std::make_shared<File>("chrome.exe", 10240, FileType::EXECUTABLE));
    programs->addChild(std::make_shared<File>("readme.txt", 1024, FileType::TEXT));

    root->addChild(docs);
    root->addChild(programs);
    disk->addChild(root);

    // Применяем различные посетители
    std::cout << "=== File System Structure ===\n";
    PrintVisitor printer;
    disk->accept(printer);

    std::cout << "\n=== Size Calculation ===\n";
    SizeCalculatorVisitor sizeCalc;
    disk->accept(sizeCalc);
    std::cout << "Total size: " << sizeCalc.getTotalSize() << " bytes\n";

    std::cout << "\n=== File Count (Multithreaded) ===\n";
    FileCounterVisitor counter;
    disk->accept(counter);
    std::cout << "Text files: " << counter.getTextFileCount() << "\n";
    std::cout << "Binary files: " << counter.getBinaryFileCount() << "\n";
    std::cout << "Executable files: " << counter.getExecutableFileCount() << "\n";

    // Пример с синхронизацией
    using SafeFolder = Synchronizable<Folder, MutexSync>;
    auto safeFolder = std::make_shared<SafeFolder>("ThreadSafeFolder");

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([safeFolder, i]() {
            auto lock = safeFolder->getLock();
            safeFolder->addChild(std::make_shared<File>(
                "thread_file_" + std::to_string(i) + ".txt",
                256,
                FileType::TEXT
            ));
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\n=== Thread-safe folder contents ===\n";
    safeFolder->accept(printer);

    return 0;
}