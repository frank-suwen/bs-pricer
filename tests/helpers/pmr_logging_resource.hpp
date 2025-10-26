#pragma once
#include <memory_resource>
#include <vector>
#include <cstddef>
#include <utility>
#include <cassert>

namespace test_pmr {

// A simple forwarding memory_resource that logs every allocate/deallocate.
// Not thread-safe; intended for unit tests.
class logging_resource final : public std::pmr::memory_resource {
public:
    struct alloc_event {
        std::size_t size;
        std::size_t alignment;
        void*       ptr;  // pointer returned by upstream
    };
    struct dealloc_event {
        std::size_t size;
        std::size_t alignment;
        void*.      ptr;  // pointer passed to deallocate
    };

    explicit logging_resource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource()) : upstream_(upstream) {
        assert(upstream_ && "upstream resource must not be null");
    }

    // Non-copyable / non-movable (simplify lifetime)
    logging_resource(const logging_resource&) = delete;
    logging_resource& operator=(const logging_resource&) = delete;

    // Observers
    const std::vector<alloc_event>& allocs() const noexcept { return allocs_; }
    const std::vector<dealloc_event>& deallocs() const noexcept { return deallocs_; }
    std::size_t bytes_allocated() const noexcept {
        std::size_t s = 0;
        for (auto& e : allocs_) s += e.size;
        return s;
    }
    std::size_t bytes_deallocated() const noexcept {
        std::size_t s = 0;
        for (auto& e : deallocs_) s += e.size;
        return s;
    }

    void clear_logs() {
        allocs_.clear();
        deallocs_.clear();
    }

protected:
    // memory_resource interface
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        void* p = upstream_->allocate(bytes, alignment);
        allocs_.push_back(alloc_event{bytes, alignment, p});
        return p;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        deallocs_.push_back(dealloc_event{bytes, alignment, p});
        upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    std::pmr::memory_resource* upstream_;
    std::vector<alloc_event>   allocs_;
    std::vector<dealloc_event> deallocs_;
}

} // namespace test_pmr