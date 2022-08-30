#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <ostream>
#include <sstream>
#include <atomic>
#include <random>

std::mutex queueLock;
std::condition_variable queueEmptyChecker;
std::queue<int> queue;
std::atomic<bool> isStopWorker(false);

void PrintMessage(const std::string &msg) {
    static std::mutex screenLock;
    std::lock_guard<std::mutex> lock(screenLock);
    std::cout << msg << std::endl;
}

/**
 * @brief Worker - генерирует сообщения и кладет их в очередь.
 */
void worker() {
    static auto counter = 0;
    while (counter < 5) {
        //! Генерируем новый элемент очереди.
        const int value = rand() % 1000;
        ++counter;

        std::stringstream oss;
        oss << "Generate new value: " << value;
        PrintMessage(oss.str());

        {
            //! Захватываем примитив синхронизации
            //! Помещаем новый элемент в очередь атомарно
            //! Затем оповещаем ожидающий поток о том что в очереди появился новый элемент и его
            //! нужно обработать.
            std::lock_guard<std::mutex> lock(queueLock);
            queue.push(value);
            queueEmptyChecker.notify_one();
        }
    }

    //! По завершению потока мы оповещаем другой поток о том что мы закончили свое исполнение
    isStopWorker.store(true);
}

/**
 * @brief Логер - осуществляет вывод данных из очереди сообщений.
 */
void logger() {
    while (!isStopWorker.load()) {
        //! необходимо захватить мютекс перед вызоввом ожидания события
        std::unique_lock<std::mutex> lock(queueLock);

        //! дополнительная проверка от ложных пробуждений
        //! она нужна, так как перед пробуждением и очередным захватам мютекса могут вклинуться другие потоки и
        //! изменить состояние условной переменной.
        //! вызов wait(): Перерд его вызовам нужно захватить примитив синхронизации. В самом методе мютекс
        //! будет освобожден (unlock). По наступлению ожидаемого события в wait происходят два действия:
        //! поток ожидающий просыпается и затем происхлдит попытка захвата мютекса. По выходу из wait
        //! поток уже имеет захваченый мютекс.
        while (queue.empty()) {
            queueEmptyChecker.wait(lock);
        }

        //! выполняем "полезную работу", т. к мы знаем что очередь н епуста(нас уведомили),
        //! мы берем елемент из очереди и выводим в лог. Делаем это пока очередь не опустеет.
        //! затем когда очередь станет пустой, мы отпускаем мютекс связанный с очередью, что бы
        //! другой поток мог добавить туда элементы.
        do {
            const auto value = queue.back();
            queue.pop();

            std::stringstream oss;
            oss << "Value from queue: " << value;
            PrintMessage(oss.str());
        } while (!queue.empty());
        lock.unlock();
    }
}

int main() {
    std::thread workerThread(worker);
    std::thread loggerThread(logger);

    workerThread.join();
    loggerThread.join();
    return 0;
}
