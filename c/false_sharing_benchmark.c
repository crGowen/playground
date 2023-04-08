#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define CACHE_LINE_SIZE 64

typedef struct {
    uint8_t* data;
    uint32_t start;
    uint32_t size;
} Args_t;

typedef struct {
    uint8_t* data;
    uint8_t offset;
    uint32_t size;
} StaggeredArgs_t;


void* ThreadFn( void* ptrArg) {
    Args_t args = *( (Args_t*)ptrArg );
    uint8_t* data = args.data + args.start;

    for (int i = 0; i < args.size; i++) {
        data[i] = data[i] * data[i];
    }
}

void* StaggeredThreadFn( void* ptrArg) {
    StaggeredArgs_t args = *( (StaggeredArgs_t*)ptrArg );
    uint8_t* data = args.data;

    for (int i = args.offset; i < args.size; i+= 4) {
        data[i] = data[i] * data[i];
    }
}

void Test(uint32_t dataSize) {
    uint8_t data[dataSize];
    // 4 threads, thread0 being the main thread
    pthread_t thread1, thread2, thread3;

    for (int i = 0; i < dataSize; i++) {
        data[i] = i % 13;
    }
    Args_t args[4];
    for (int i = 0; i < 4; i++) {
        args[i] = (Args_t){
            .data = data,
            .start = i * (dataSize/4),
            .size = dataSize/4
        };
    }
    clock_t timer = clock();
    pthread_create(&thread1, NULL, ThreadFn,(void*)(args + 1));
    pthread_create(&thread2, NULL, ThreadFn,(void*)(args + 2));
    pthread_create(&thread3, NULL, ThreadFn,(void*)(args + 3));
    ThreadFn((void*)(args));

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    timer = clock() - timer;
    printf("CPU time: %lu, data size: %u\n", timer, dataSize);
}

void StaggeredTest (uint32_t dataSize) {
    uint8_t data[dataSize];
    // 4 threads, thread0 being the main thread
    pthread_t thread1, thread2, thread3;

    for (int i = 0; i < dataSize; i++) {
        data[i] = i % 13;
    }
    StaggeredArgs_t args[4];
    for (int i = 0; i < 4; i++) {
        args[i] = (StaggeredArgs_t){
            .data = data,
            .offset = i,
            .size = dataSize
        };
    }
    clock_t timer = clock();
    pthread_create(&thread1, NULL, StaggeredThreadFn,(void*)(args + 1));
    pthread_create(&thread2, NULL, StaggeredThreadFn,(void*)(args + 2));
    pthread_create(&thread3, NULL, StaggeredThreadFn,(void*)(args + 3));
    ThreadFn((void*)(args));

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    timer = clock() - timer;
    printf("CPU time: %lu, data size: %u (staggered)\n", timer, dataSize);
}

int main() {
    //uint8_t data[4*CACHE_LINE_SIZE]; // for the ideal situation, the data size would be a multiple of number of threads * cache line length

    for (int i = 0; i < 50; i++) {
        // bad performance, with some cache line overlap
        Test(255);
        // bad performance, with some cache line overlap
        Test(257);
        // bad performance, with some cache line overlap
        Test(250);

        // all numbers close to ideal size of 25600, but will incur cache line overlap / false sharing to some extent

        // optimal performance, because the data size is a multiple of cache line length and number of threads (4), there is not overlap
        Test(4*CACHE_LINE_SIZE * 100); // equivalent to Test(256) on my 64B cache line system
        StaggeredTest(4*CACHE_LINE_SIZE * 100);
        printf("\n");
    }

    for (int i = 0; i < 5; i++) {
        // optimal performance, because the data size is a multiple of cache line length and number of threads (4), there is not overlap
        Test(4*CACHE_LINE_SIZE * 100); // equivalent to Test(256) on my 64B cache line system

        //Staggered cache, to demonstrate the worst case scenario for false sharing
        StaggeredTest(4*CACHE_LINE_SIZE * 100);
        printf("\n");
    }


    return 0;
}