#pragma once

#include "display.hpp"

class display_win : public display_impl
{
public:
	void init();
	void writeln(const std::string&) override;
	void clear();
	void check_size();

	int colums;
	int rows;
private:
	void* hstd;
	void* og_mode;
};