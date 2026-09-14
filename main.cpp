#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "monitor.h"
using namespace std;
static Monitor mon;
int n = 0;
void* provider(void* arg) {
    for (int i = 1;i <= 10;i++) {
        sleep(1);
        n = i;
        mon.provide(n);
    }
    return nullptr;
}
void* consumer(void* arg) {
    for (int i = 0;i < 10;i++) {
        int res = mon.consume();
        sleep(1); // long calculations
        cout << res << endl;
    }

    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, consumer, nullptr);
    sleep(2);
    pthread_create(&t2, nullptr, provider, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    return 0;
}