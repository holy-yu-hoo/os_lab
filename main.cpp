#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

const char* const dir = "/data";
const char* const lock_path = "/data/.lock";


int get_file(char* name, int& file_number) {
    for (file_number = 1; file_number <= 999; ++file_number) {
        char path[64];
        std::snprintf(path, sizeof(path), "%s/file_%03d", dir, file_number);
        const int file = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644);

        if (file >= 0) {
            std::strcpy(name, path);
            return file;
        }
    }

    return -1;
}

int main() {
    const char* container_id = std::getenv("CONTAINER_ID");
    int f_num;
    char f_name[64];
    int created_number = 1;
    const int lock_file = open(lock_path, O_RDWR | O_CREAT, 0644);
    if (lock_file < 0) {
        std::cerr << container_id << ": cant open " << lock_path << std::endl;
        return 1;
    }

    while (true) {
        if (flock(lock_file, LOCK_EX) != 0) {
            std::cerr << container_id << ": cant lock " << lock_path << std::endl;
            close(lock_file);
            return 1;
        }

        const int file = get_file(f_name, f_num);

        if (file < 0) {
            std::cerr << container_id << ": cant create file" << std::endl;
            flock(lock_file, LOCK_UN);
            close(lock_file);
            return 1;
        }

        dprintf(file, "container id: %s\nfile number: %d\n", container_id, created_number++);
        close(file);

        if (flock(lock_file, LOCK_UN) != 0) {
            std::cerr << container_id << ": cant unlock " << lock_path << std::endl;
            close(lock_file);
            return 1;
        }

        sleep(1);

        if (flock(lock_file, LOCK_EX) != 0) {
            std::cerr << container_id << ": cant lock " << lock_path << std::endl;
            close(lock_file);
            return 1;
        }

        std::remove(f_name);
        if (flock(lock_file, LOCK_UN) != 0) {
            std::cerr << container_id << ": cant unlock " << lock_path << std::endl;
            close(lock_file);
            return 1;
        }

    }
}
