#ifndef TVM_RUNTIME_MEMORY_BEYOND_ALLOCATOR_H_
#define TVM_RUNTIME_MEMORY_BEYOND_ALLOCATOR_H_

#include <tvm/runtime/device_api.h>
#include <tvm/runtime/memory/memory_manager.h>
#include "allocator_util.h"

#include <atomic>
#include <string>

namespace tvm {
namespace runtime {
namespace memory {

MyMemoryManager beyondMemoryManager(10 * 1024 * 1024);
static int i = 0;

class BEYONDAllocator final : public Allocator {
  public:
  explicit BEYONDAllocator(Device dev) : Allocator(kBeyond), used_memory_(0), device_(dev) {}

  Buffer Alloc(size_t nbytes, size_t alignment, DLDataType type_hint) override {
    Buffer buf;
    buf.device = device_;
    buf.size = nbytes;
    buf.alloc_type = kBeyond;
    buf.data = beyondMemoryManager.myMalloc("NPU"+std::to_string(++i), nbytes);
    beyondMemoryManager.printMemoryStatus();
    used_memory_.fetch_add(nbytes, std::memory_order_relaxed);
    std::cout << buf.data << ", " << buf.size << ", " << buf.device << "\n";
    DLOG(INFO) << "allocate " << nbytes << " B, used memory " << used_memory_ << " B";
    return buf;
  }

  Buffer Alloc(ShapeTuple shape, DLDataType type_hint, const std::string& mem_scope) override {
    Buffer buf;
    size_t nbytes = 1;
    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
      nbytes *= static_cast<size_t>(shape[i]);
    }
    nbytes *= (type_hint.bits * type_hint.lanes + 7) / 8;
    buf.device = device_;
    if (mem_scope.empty() || mem_scope == "global") {
      auto tmp_buf = Allocator::Alloc(device_, shape, type_hint, mem_scope);
      buf.size = tmp_buf.size;
      buf.data = tmp_buf.data;
      buf.alloc_type = kBeyond;
      return buf;
    }
    buf.size = nbytes;
    buf.data = DeviceAPI::Get(device_)->AllocDataSpace(device_, shape.size(), shape.data(),
                                                       type_hint, String(mem_scope));
    used_memory_.fetch_add(nbytes, std::memory_order_relaxed);
    DLOG(INFO) << "allocate " << nbytes << " B, used memory " << used_memory_ << " B";
    buf.alloc_type = kBeyond;
    return buf;
  }

  void Free(const Buffer& buffer) override {
    // DeviceAPI::Get(device_)->FreeDataSpace(buffer.device, buffer.data);
    beyondMemoryManager.myFree(buffer.data);
    used_memory_.fetch_sub(buffer.size, std::memory_order_relaxed);
    DLOG(INFO) << "free " << buffer.size << " B, used memory " << used_memory_ << " B";
  }

  size_t UsedMemory() const override { return used_memory_.load(std::memory_order_relaxed); }

 private:
  std::atomic<size_t> used_memory_;
  Device device_;
};

}  // namespace memory
}  // namespace runtime
}  // namespace tvm

#endif  // TVM_RUNTIME_MEMORY_BEYOND_ALLOCATOR_H_