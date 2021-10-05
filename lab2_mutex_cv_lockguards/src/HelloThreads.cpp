#include <iostream>
#include <thread>

//using namespace std;

void fn1(int x) {
    std::cout << "in fn1: " << x << std::endl;
}

void fn2(int &x) {
    std::cout << "in fn2: " << x << std::endl;
    x = 100;    // Change value of xMain.
}

class MyClass {
public:
    static void fn3(int x) {
        std::cout << "in fn3: " << x << std::endl;
    }
};

int main() {
    std::thread thread1(fn1, 10);    // Pass argument by value
    thread1.join();

    int xMain = 20;
    std::thread thread2(fn2, std::ref(xMain));    // Pass argument by reference.
    thread2.join();
    std::cout << "xMain after thread2 ends: " << xMain << std::endl;

    MyClass myClass;
    std::thread thread3(&MyClass::fn3, &myClass, 123);    // Class method as thread.
    thread3.join();

    return 0;
}