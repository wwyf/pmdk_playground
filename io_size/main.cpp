#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <libpmemobj.h>

// uint64_t block_size = 64; // 32bytes
uint64_t block_size = 128; // 1MB
// const uint64_t block_size = 1024*1024*1; // 1MB
// uint64_t count = 1*1024 * 8; 
uint64_t count = 1*1024 * 1024 * 2; 
// const uint64_t count = 8; 
uint64_t data_space_size = block_size*count; // 8GB

PMEMobjpool * pop = NULL;

uint64_t test_write_pm(size_t io_size, uint8_t * data_space_ptr){
    // return latency

    uint8_t * dummy_data = (uint8_t*)malloc(io_size);
    for (uint64_t i = 0; i < io_size; i++){
        dummy_data[i] = (random()+i) % 256;
    }

    uint64_t offset = 0;
    uint64_t duration; //ns
    auto start_time = std::chrono::system_clock::now();

    while(offset < data_space_size){
        // memcpy(data_space_ptr+offset, dummy_data, io_size);
        // pmemobj_persist(pop, data_space_ptr+offset,io_size);
        // pmemobj_memcpy_persist(pop, data_space_ptr+offset, dummy_data ,io_size);
        pmemobj_memcpy(pop, data_space_ptr+offset, dummy_data ,io_size, PMEMOBJ_F_MEM_TEMPORAL);
        offset += block_size;
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_time).count();
    
    // std::cout << duration << std::endl;

    return duration;
}
uint64_t test_read_pm(size_t io_size, uint8_t * data_space_ptr){
    // return latency

    uint8_t * dummy_data = (uint8_t*)malloc(io_size);

    uint64_t offset = 0;
    uint64_t duration; //ns
    auto start_time = std::chrono::system_clock::now();

    while(offset < count*block_size){
        memcpy(dummy_data, data_space_ptr+offset, io_size);
        // pmemobj_persist(pop, data_space_ptr+offset  ,io_size);
        // pmemobj_memcpy_persist(pop, dummy_data,  data_space_ptr+offset  ,io_size);
        // pmemobj_memcpy(pop, data_space_ptr+offset, dummy_data ,io_size, PMEMOBJ_F_MEM_TEMPORAL);
        // pmemobj_memcpy(pop, data_space_ptr+offset, dummy_data ,io_size, PMEMOBJ_F_MEM_TEMPORAL);
        offset += block_size;
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_time).count();
    
    // std::cout << duration << std::endl;

    return duration;
}

double test_write_dram(size_t io_size, uint8_t * data_space_ptr){
    // return latency

    uint8_t * dummy_data = (uint8_t*)malloc(io_size);
    for (uint64_t i = 0; i < io_size; i++){
        dummy_data[i] = (random()+i) % 256;
    }

    int i = 0;
    uint64_t offset = 0;
    uint64_t duration; //ns
    auto start_time = std::chrono::system_clock::now();

    while(i < count){
        memcpy(data_space_ptr+offset, dummy_data , io_size);
        offset += block_size;
        i++;
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_time).count();
    

    return (double)duration/count;
}

// double test_write_pm_2(size_t io_size, uint8_t * data_space_ptr){
//     // return latency

//     uint8_t * dummy_data = (uint8_t*)malloc(io_size);
//     for (uint64_t i = 0; i < io_size; i++){
//         dummy_data[i] = (random()+i) % 256;
//     }

//     uint64_t offset = 0;
//     uint64_t offsets[count];
//     int i = 0;
//     while(i < count){
//         offsets[i] = offset;
//         offset += block_size;
//         i++;
//     }
//     uint64_t duration; //ns
//     i = 0;
//     auto start_time = std::chrono::system_clock::now();

//     while(i < count){
//         pmemobj_memcpy_persist(pop, data_space_ptr+offsets[i],dummy_data , io_size);
//         i++;
//     }

//     duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
//         std::chrono::system_clock::now() - start_time).count();
    

//     return (double)duration/count;
// }

uint64_t test_random_write_pm(size_t io_size, uint8_t * data_space_ptr){
    // return latency

    uint8_t * dummy_data = (uint8_t*)malloc(io_size);
    for (uint64_t i = 0; i < io_size; i++){
        dummy_data[i] = (random()+i) % 256;
    }

    std::vector<uint64_t> offsets;
    int i = 0;
    for (int i = 0; i < count; i++){
        offsets.push_back(i*block_size);
    }
    // std::cout << "before random shuffle : ";
    // for (int i = 0; i < 10; i++){
    //     std::cout << offsets[i] << " ";
    // }
    // std::cout << std::endl;
    std::random_shuffle(offsets.begin(), offsets.end());
    // std::cout << "after random shuffle : ";
    // for (int i = 0; i < 10; i++){
    //     std::cout << offsets[i] << " ";
    // }
    // std::cout << std::endl;


    uint64_t duration; //ns
    i = 0;
    auto start_time = std::chrono::system_clock::now();

    while(i < count){
        pmemobj_memcpy_persist(pop, data_space_ptr+offsets[i], dummy_data , io_size);
        i++;
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_time).count();
 

    return duration;
}


uint64_t test_random_read_pm(size_t io_size, uint8_t * data_space_ptr){
    // return latency

    uint8_t * dummy_data = (uint8_t*)malloc(io_size);
    for (uint64_t i = 0; i < io_size; i++){
        dummy_data[i] = (random()+i) % 256;
    }

    std::vector<uint64_t> offsets;
    int i = 0;
    for (int i = 0; i < count; i++){
        offsets.push_back(i*block_size);
    }
    // std::cout << "before random shuffle : ";
    // for (int i = 0; i < 10; i++){
    //     std::cout << offsets[i] << " ";
    // }
    // std::cout << std::endl;
    std::random_shuffle(offsets.begin(), offsets.end());
    // std::cout << "after random shuffle : ";
    // for (int i = 0; i < 10; i++){
    //     std::cout << offsets[i] << " ";
    // }
    // std::cout << std::endl;


    uint64_t duration; //ns
    i = 0;
    auto start_time = std::chrono::system_clock::now();

    while(i < count){
        pmemobj_memcpy_persist(pop, dummy_data ,data_space_ptr+offsets[i],  io_size);
        i++;
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_time).count();
 

    return duration;
}

int test_pm(const std::vector<int> & io_sizes){
    uint64_t repeated = 1;
    // init pm
    std::string pool_file = "/mnt/pmem/test_write_pool";
    std::remove(pool_file.c_str());
    pop = pmemobj_create(pool_file.c_str(), "TEST_WRITE", 1024ULL*1024ULL*1024ULL*20ULL, 0066);
    PMEMoid data_space;
    if(pmemobj_alloc(pop, &data_space, data_space_size, 0,0, NULL)){
        std::cout << "alloc error" << std::endl;
        std::remove(pool_file.c_str());
        return -1;
    }
    // uint8_t * data_space_ptr = (uint8_t*)pmemobj_direct(data_space) + 0ULL;
    uint8_t * data_space_ptr = (uint8_t*)pmemobj_direct(data_space) + 48ULL;

    test_write_pm(8, data_space_ptr);

    for (auto io_size : io_sizes){
        uint64_t duration = 0;
        for (uint64_t i = 0; i < repeated; i++){
            duration += test_write_pm(io_size, data_space_ptr);
            // latency += test_write_pm(io_size, data_space_ptr);
            // duration += test_random_write_pm(io_size, data_space_ptr);
        }
        double avg_latency = (double)duration/(repeated*count);
        uint64_t throughput = 1000*1000*1000/avg_latency;
        uint64_t bandwidth = io_size*throughput/(1024*1024);
        std::cout << "write"
        << "," << "seq"
        << "," << block_size
        << "," << count
        << "," << data_space_size 
        << "," << io_size
        << "," << avg_latency
        << "," << throughput
        << "," << bandwidth
        << std::endl;
    }

  
    for (auto io_size : io_sizes){
        uint64_t duration = 0;
        for (uint64_t i = 0; i < repeated; i++){
            duration += test_read_pm(io_size, data_space_ptr);
            // latency += test_write_pm(io_size, data_space_ptr);
            // duration += test_random_read_pm(io_size, data_space_ptr);
        }
        double avg_latency = (double)duration/(repeated*count);
        uint64_t throughput = 1000*1000*1000/avg_latency;
        uint64_t bandwidth = io_size*throughput/(1024*1024);
        std::cout << "read"
        << "," << "seq"
        << "," << block_size
        << "," << count
        << "," << data_space_size 
        << "," << io_size
        << "," << avg_latency
        << "," << throughput
        << "," << bandwidth
        << std::endl;
    }

    pmemobj_free(&data_space);
    pmemobj_close(pop);

    // close bench
    std::remove(pool_file.c_str());
    return 0;
}

int test_random_pm(const std::vector<int> & io_sizes){
    uint64_t repeated = 1;
    // init pm
    std::string pool_file = "/mnt/pmem/test_write_pool";
    std::remove(pool_file.c_str());
    pop = pmemobj_create(pool_file.c_str(), "TEST_WRITE", 1024ULL*1024ULL*1024ULL*20ULL, 0066);
    PMEMoid data_space;
    if(pmemobj_alloc(pop, &data_space, data_space_size, 0,0, NULL)){
        std::cout << "alloc error" << std::endl;
        std::remove(pool_file.c_str());
        return -1;
    }
    // uint8_t * data_space_ptr = (uint8_t*)pmemobj_direct(data_space) + 0ULL;
    uint8_t * data_space_ptr = (uint8_t*)pmemobj_direct(data_space) + 48ULL;

    test_write_pm(8, data_space_ptr);

    for (auto io_size : io_sizes){
        uint64_t duration = 0;
        for (uint64_t i = 0; i < repeated; i++){
            // duration += test_write_pm(io_size, data_space_ptr);
            // latency += test_write_pm(io_size, data_space_ptr);
            duration += test_random_write_pm(io_size, data_space_ptr);
        }
        double avg_latency = (double)duration/(repeated*count);
        uint64_t throughput = 1000*1000*1000/avg_latency;
        uint64_t bandwidth = io_size*throughput/(1024*1024);
        std::cout << "write"
        << "," << "random"
        << "," << block_size
        << "," << count
        << "," << data_space_size 
        << "," << io_size
        << "," << avg_latency
        << "," << throughput
        << "," << bandwidth
        << std::endl;
    }

  
    for (auto io_size : io_sizes){
        uint64_t duration = 0;
        for (uint64_t i = 0; i < repeated; i++){
            // duration += test_read_pm(io_size, data_space_ptr);
            // latency += test_write_pm(io_size, data_space_ptr);
            duration += test_random_read_pm(io_size, data_space_ptr);
        }
        double avg_latency = (double)duration/(repeated*count);
        uint64_t throughput = 1000*1000*1000/avg_latency;
        uint64_t bandwidth = io_size*throughput/(1024*1024);
        std::cout << "read"
        << "," << "random"
        << "," << block_size
        << "," << count
        << "," << data_space_size 
        << "," << io_size
        << "," << avg_latency
        << "," << throughput
        << "," << bandwidth
        << std::endl;
    }

    pmemobj_free(&data_space);
    pmemobj_close(pop);

    // close bench
    std::remove(pool_file.c_str());
    return 0;
}






int test_dram(const std::vector<int> & io_sizes){
    uint64_t repeated = 5000;

    uint8_t * data_space_ptr = (uint8_t*)malloc(1024ULL*1024ULL*1024ULL*20ULL);

    std::cout << "io_size(Bytes),write_avg_latency(ns),throughput,bandwidth(MB)" << std::endl;

    for (auto io_size : io_sizes){
        double latency = 0;
        for (uint64_t i = 0; i < repeated; i++){
            // latency += test_write_pm(io_size, data_space_ptr);
            // latency += test_write_pm(io_size, data_space_ptr);
            latency += test_write_dram(io_size, data_space_ptr);
        }
        double avg_latency = latency/repeated;
        uint64_t throughput = 1000*1000*1000/avg_latency;
        uint64_t bandwidth = io_size*throughput/(1024*1024);
        std::cout << io_size
        << "," << avg_latency
        << "," << throughput
        << "," << bandwidth
        << std::endl;
    }
    return 0;
}


int main(){
    // init io_sizes
    // uint64_t l = 5; // 2^3 8Bytes
    uint64_t l = 5; // 2^5 32Bytes
    // uint64_t l = 4; // 2^4 16Bytes
    uint64_t h = 5; // 2^3 8B
    // uint64_t h = 9; // 2^9 512B
    // uint64_t h = 16; // 2^16 64KB
    // uint64_t h = 18; // 2^18 256KB
    // uint64_t h = 19; // 2^19 512KB
    // uint64_t h = 20; // 2^20 1MB
    // uint64_t h = 21; // 2^21 2MB
    // uint64_t h = 22; // 2^20 4MB
    uint64_t start_io_size = 32;
    
    std::vector<int> io_sizes;
    for (;l <= h; l++){
        io_sizes.push_back(start_io_size);
        start_io_size *= 2;
    }
    std::cout << data_space_size << std::endl;
    int max_count_i = 1;
    count = 1024*8;

    std::cout << "op,workload_type,block_size,count,data_space_size,io_size(Bytes),avg_latency(ns),throughput,bandwidth(MB)" << std::endl;

    for (block_size = 32; block_size <= 1024*1024; block_size*=2){
        data_space_size = count * block_size;
        // std::cout << "count : " <<  count << std::endl;
        // std::cout << "data_space_size : " << data_space_size << std::endl;
        test_pm(io_sizes);
        sleep(2);
        // test_random_pm(io_sizes);
        // sleep(2);
    }
    for (block_size = 32; block_size <= 1024*1024; block_size*=2){
        data_space_size = count * block_size;
        // std::cout << "count : " <<  count << std::endl;
        // std::cout << "data_space_size : " << data_space_size << std::endl;
        // test_pm(io_sizes);
        // sleep(2);
        test_random_pm(io_sizes);
        sleep(2);
    }

    // test_dram(io_sizes);

    return 0;
}