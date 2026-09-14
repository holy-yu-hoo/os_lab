#include "monitor.h"
#include <iostream>
#include <unistd.h>

using namespace std;
Monitor::Monitor() {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
}

Monitor::~Monitor() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void Monitor::provide(int p_data) {
    cout << "provide lock" << endl;
    pthread_mutex_lock(&mutex);
    // if (!q.empty()) {
    //     cout << "provide unlock because state=1" << endl;
    //     pthread_mutex_unlock(&mutex);
    //     return;
    // }
    cout << "provide set data" << endl;
    q.push(p_data);
    cout << "provide signal" << endl;
    pthread_cond_signal(&cond);
    cout << "provide unlock" << endl;
    pthread_mutex_unlock(&mutex);
}

int Monitor::consume() {
    cout << "consume lock" << endl;
    pthread_mutex_lock(&mutex);
    while (q.empty()) {
        cout << "consume wait because state=0" << endl;
        pthread_cond_wait(&cond, &mutex);
    }
    cout << "consume get data" << endl;

    int result = q.front();
    q.pop();
    state = false;
    cout << "consume unlock" << endl;
    pthread_mutex_unlock(&mutex);
    return result;

}