#pragma once
#include <Windows.h>
#include <string>
#include <mutex>
__declspec(noinline) void raw_buffer_print(const char* data, size_t len);
// fast print  C++
std::mutex deepPayloadMt;

template <class... Tm>
void print(const std::format_string<Tm...> _Fmt, Tm&&... _Args) {
    std::lock_guard<std::mutex> lock(deepPayloadMt);
    std::string_view out_buffer = _STD vformat(_Fmt.get(), _STD make_format_args(_Args...));
    raw_buffer_print(out_buffer.data(), out_buffer.size());
}
__declspec(noinline) void raw_buffer_print(const char* data, size_t len) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD written;
    WriteFile(hOut, data, (DWORD)len, &written, NULL);
}