#ifdef PERF
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

struct PerfCounters {
    struct {
        int fd;
        uint64_t value;
    } l1_misses, llc_misses, branches, branch_misses, instructions, cycles;

    void start() {
        ioctl(l1_misses.fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(llc_misses.fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(branches.fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(branch_misses.fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(instructions.fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(cycles.fd, PERF_EVENT_IOC_RESET, 0);
        
        ioctl(l1_misses.fd, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(llc_misses.fd, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(branches.fd, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(branch_misses.fd, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(instructions.fd, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(cycles.fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    void stop() {
        ioctl(l1_misses.fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(llc_misses.fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(branches.fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(branch_misses.fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(instructions.fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(cycles.fd, PERF_EVENT_IOC_DISABLE, 0);

        read(l1_misses.fd, &l1_misses.value, sizeof(uint64_t));
        read(llc_misses.fd, &llc_misses.value, sizeof(uint64_t));
        read(branches.fd, &branches.value, sizeof(uint64_t));
        read(branch_misses.fd, &branch_misses.value, sizeof(uint64_t));
        read(instructions.fd, &instructions.value, sizeof(uint64_t));
        read(cycles.fd, &cycles.value, sizeof(uint64_t));
    }

    void print() const {
        const double l1_hit_rate = 1.0 - (double)l1_misses.value / instructions.value;
        const double llc_hit_rate = 1.0 - (double)llc_misses.value / instructions.value;
        const double branch_miss_rate = (double)branch_misses.value / branches.value;

        printf("Perf counters:\n");
        printf("L1 misses: %lu (Hit rate: %.2f%%)\n", l1_misses.value, l1_hit_rate*100);
        printf("LLC misses: %lu (Hit rate: %.2f%%)\n", llc_misses.value, llc_hit_rate*100);
        printf("Branches: %lu (Miss rate: %.2f%%)\n", branches.value, branch_miss_rate*100);
        printf("Instructions: %lu\n", instructions.value);
        printf("Cycles: %lu (IPC: %.2f)\n", cycles.value, (double)instructions.value/cycles.value);
    }
};

PerfCounters setup_perf() {
    PerfCounters pc = {};
    struct perf_event_attr attr = {};
    
    auto open_counter = [&](uint64_t type, uint64_t config) {
        attr.type = type;
        attr.size = sizeof(attr);
        attr.config = config;
        attr.disabled = 1;
        attr.exclude_kernel = 1;
        return syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
    };

    pc.l1_misses.fd = open_counter(PERF_TYPE_HW_CACHE, 
        PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16));
    
    pc.llc_misses.fd = open_counter(PERF_TYPE_HW_CACHE,
        PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16));
    
    pc.branches.fd = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
    pc.branch_misses.fd = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);
    pc.instructions.fd = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
    pc.cycles.fd = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    return pc;
}

#endif