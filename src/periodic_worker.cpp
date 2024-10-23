#include <spdlog/details/periodic_worker.h>

namespace spdlog::details {
    // stop the worker thread and join it
    periodic_worker::~periodic_worker() {
        if (worker_thread_.joinable()) {
            {
                std::lock_guard lock(mutex_);
                active_ = false;
            }
            cv_.notify_one();
            worker_thread_.join();
        }
    }
}
