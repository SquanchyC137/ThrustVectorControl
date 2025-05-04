#include <iostream>
#include <windows.h>

void printProgressBar(int progress, int total) {
    const int barWidth = 30;
    float ratio = float(progress) / total;
    int pos = barWidth * ratio;

    std::cout << "\r[";
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(ratio * 100) << "%";

    if (progress == total) {
        std::cout << "\nSETUP FINISHED.\n";
    }
}

int main() {
    for (int i = 0; i <= 100; i++) {
        printProgressBar(i, 100);
        Sleep(100);  // 100ms Delay
    }
    return 0;
}
