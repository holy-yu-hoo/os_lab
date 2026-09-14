#include <pthread.h>
#include <queue>
class Monitor {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool state;
    void* data;
    std::queue<int> q;

public:
    void provide(int p_data);
    int consume();

    Monitor();
    ~Monitor();
};
