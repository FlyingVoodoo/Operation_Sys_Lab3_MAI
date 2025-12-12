#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstring>
#include <sys/wait.h>

#include "os.hpp"

SharedData* CreateSharedMemory() {
    shm_unlink("/shared");
    int shm_fd = shm_open("/shared", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); exit(1); }
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData* data = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); exit(1); }
    close(shm_fd);

    sem_unlink("/sem_file");
    sem_unlink("/sem_parent");
    sem_unlink("/sem_child");
    
    data->sem_file = sem_open("/sem_file", O_CREAT, 0666, 1);
    data->sem_parent = sem_open("/sem_parent", O_CREAT, 0666, 0);
    data->sem_child = sem_open("/sem_child", O_CREAT, 0666, 0);
    
    if (data->sem_file == SEM_FAILED || data->sem_parent == SEM_FAILED || data->sem_child == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    return data;
}

SharedData* OpenSharedMemory() {
    int shm_fd = shm_open("/shared", O_RDWR, 0);
    if (shm_fd == -1) { perror("shm_open"); exit(1); }
    SharedData* data = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); exit(1); }
    close(shm_fd);
    
    data->sem_file = sem_open("/sem_file", 0);
    data->sem_parent = sem_open("/sem_parent", 0);
    data->sem_child = sem_open("/sem_child", 0);
    
    if (data->sem_file == SEM_FAILED || data->sem_parent == SEM_FAILED || data->sem_child == SEM_FAILED) {
        perror("sem_open in OpenSharedMemory");
        exit(1);
    }
    
    return data;
}

void DestroySharedMemory(SharedData* data) {
    sem_close(data->sem_file);
    sem_close(data->sem_parent);
    sem_close(data->sem_child);
    
    sem_unlink("/sem_file");
    sem_unlink("/sem_parent");
    sem_unlink("/sem_child");
    
    munmap(data, sizeof(SharedData));
    shm_unlink("/shared");
}

ProcessRole ProcessCreate() {
    pid_t pid = fork();
    if (pid == -1) { std::cout << "Ошибка создания process" << std::endl; exit(-1); }
    if (pid == 0) { return IS_CHILD; }
    return IS_PARENT;
}

void ProcessExecute(const char* program, const char* arg) {
    execl(program, arg, NULL);
    perror("execl");
    exit(1);
}

pid_t ProcessWait(pid_t pid) {
    int status;
    return waitpid(pid, &status, 0);
}

void SemaphorePost(sem_t* sem) {
    if (sem_post(sem) == -1) { perror("sem_post"); exit(1); }
}

void SemaphoreWait(sem_t* sem) {
    if (sem_wait(sem) == -1) { perror("sem_wait"); exit(1); }
}

char* MapFileContent(const char* filename, size_t size, int flags) {
    int fd = open(filename, flags, 0666);
    if (fd == -1) {
        perror("open for file mapping");
        return nullptr;
    }

    if (ftruncate(fd, size) == -1 && (flags & O_CREAT)) {
        close(fd);
        perror("ftruncate");
        return nullptr;
    }

    char* mapped_addr = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (mapped_addr == MAP_FAILED) {
        perror("mmap file content");
        return nullptr;
    }
    return mapped_addr;
}