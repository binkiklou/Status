#pragma once

#include <string>

class system_impl
{
public:
	virtual bool start(); // Returns true if succesfull
	virtual void stop();
	virtual std::wstring get_property(const std::string&);
};

class system_info
{
public:
	system_info();
	~system_info();

	void get_cpu();
private:
	// TODO: unique_ptr
	system_impl* m_impl;
};