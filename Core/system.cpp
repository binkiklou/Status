#include "system.hpp"

#include <new>
#include <iostream>

#ifdef _WIN32
#include "system_win.hpp"
#endif

//===================================================
// Doesn't work on some compilers unless defined
bool system_impl::start() { return false; }
void system_impl::stop() {}
std::wstring system_impl::get_property(const std::string& a, const std::wstring& b) { return L"err"; }
//===================================================

system_info::system_info()
{
#ifdef _WIN32
    this->m_impl = new system_win;
    static_cast<system_win*>(this->m_impl)->start();
#else
    std::cout << "No implementation for this plattform." << std::endl;
#endif
}

system_info::~system_info()
{
    this->m_impl->stop();
}

void system_info::get_cpu()
{
    std::wcout<<"Property:"<<this->m_impl->get_property("WIN32_OperatingSystem", L"Caption") << std::endl;
}