#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
