#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>

#include "os.hpp"
#include "shared_data.hpp"

const size_t FILE_SIZE = 4096;
const char* DATA_FILENAME = "data_to_process.txt";

int main() {
    SharedData* control_data = CreateSharedMemory();
    strncpy(control_data->file, DATA_FILENAME, sizeof(control_data->file));
    control_data->file[sizeof(control_data->file) - 1] = '\0';
    char* file_content = MapFileContent(DATA_FILENAME, FILE_SIZE, O_CREAT | O_RDWR);
    if (!file_content) {
        DestroySharedMemory(control_data);
        return -1;
    }
    std::string input_string;
    std::cout << "Enter a string: ";
    std::getline(std::cin, input_string);
    
    strncpy(file_content, input_string.c_str(), FILE_SIZE - 1);
    file_content[FILE_SIZE - 1] = '\0';
    pid_t pid1 = 0;
    if (ProcessCreate() == IS_CHILD) {
        ProcessExecute("./child1", "child1");
    }
    pid1 = getpid(); 

    pid_t pid2 = 0;
    if (ProcessCreate() == IS_CHILD) {
        ProcessExecute("./child2", "child2");
    }
    pid2 = getpid(); 
    SemaphorePost(control_data->sem_child); 
    SemaphoreWait(control_data->sem_parent); 
    std::cout << "Final output: " << file_content << std::endl;
    munmap(file_content, FILE_SIZE);
    unlink(DATA_FILENAME); 
    
    ProcessWait(pid1);
    ProcessWait(pid2);
    
    DestroySharedMemory(control_data);
    
    return 0;
}