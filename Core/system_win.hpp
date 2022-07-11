#pragma once

#include "system.hpp"

#include <string>
#include <thread>

class system_win : public system_impl
{
public:
	bool start() override;
	void stop() override;
	std::wstring get_property(const std::string&) override;
private:
	void* m_pLoc = NULL;
	void* m_pSvc = NULL;
};