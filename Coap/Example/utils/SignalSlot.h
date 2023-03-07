#pragma once

#include <functional>
#include <vector>

template <typename... Args>
class Signal {
public:
    using SlotFunction = std::function<void(Args...)>;

    void connect(SlotFunction slot) {
        slots_.push_back(slot);
    }

    void emit(Args... args) {
        for (auto& slot : slots_) {
            slot(args...);
        }
    }

private:
    std::vector<SlotFunction> slots_;
};

// // 示例用法
// int main() {
//     Signal<int> signal;
//     signal.connect([](int arg) {
//         std::cout << "Received signal with arg: " << arg << std::endl;
//     });
//     signal.emit(42); // Output: Received signal with arg: 42
//     return 0;
// }
