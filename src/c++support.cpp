#include <cstddef>

extern "C" void __wasm_call_ctors();

[[clang::export_name("_initialize")]]
void _initialize() {
    __wasm_call_ctors();
}

extern "C" {

void* __cxa_allocate_exception(size_t) { __builtin_trap(); return nullptr; }
void __cxa_throw(void *, void *, void (*) (void *)) { __builtin_trap(); }

}
