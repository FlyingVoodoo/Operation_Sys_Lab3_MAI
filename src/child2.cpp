#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>

#include "os.hpp"
#include "shared_data.hpp"

const size_t FILE_SIZE = 4096;

int main() {
    SharedData* control_data = OpenSharedMemory();
    SemaphoreWait(control_data->sem_child); 
    SemaphoreWait(control_data->sem_file);
    char* file_content = MapFileContent(control_data->file, FILE_SIZE, O_RDWR);
    if (!file_content) {
        SemaphorePost(control_data->sem_file); 
        return -1;
    }
    size_t len = strlen(file_content);
    for (size_t i = 0; i < len; ++i) {
        if (file_content[i] == ' ') {
            file_content[i] = '_';
        }
    }
    munmap(file_content, FILE_SIZE);
    SemaphorePost(control_data->sem_file);
    SemaphorePost(control_data->sem_parent);
    munmap(control_data, sizeof(SharedData));
    return 0;
}