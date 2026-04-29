#include <libgo/libgo.h>

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

int main() {
    co_chan<int> jobs(2);
    co_mutex result_mutex;
    std::vector<std::string> trace;
    int sum = 0;

    go [jobs] {
        for (int i = 1; i <= 3; ++i) {
            jobs << i;
            co_sleep(10);
        }
    };

    go [jobs] {
        for (int i = 4; i <= 6; ++i) {
            jobs << i;
            co_sleep(10);
        }
    };

    go [jobs, &result_mutex, &trace, &sum] {
        for (int i = 0; i < 6; ++i) {
            int value = 0;
            jobs >> value;

            {
                std::lock_guard<co_mutex> lock(result_mutex);
                sum += value;
                trace.push_back("consume " + std::to_string(value));
            }
        }

        {
            std::lock_guard<co_mutex> lock(result_mutex);
            std::cout << "trace:" << std::endl;
            for (const auto& line : trace) {
                std::cout << "  " << line << std::endl;
            }
            std::cout << "sum = " << sum << std::endl;
        }

        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
