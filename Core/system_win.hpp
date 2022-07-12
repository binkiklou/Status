#pragma once

#include "system.hpp"

#include <string>
#include <thread>
#include <vector>

class system_win : public system_impl
{
public:
	bool start() override;
	void stop() override;
	std::wstring get_property(const std::string&, const std::wstring&, int) override;
	
	bool connect(const std::wstring&);
	void disconnect(int);
private:
	std::vector<std::pair<void*, void*>> connections;
};