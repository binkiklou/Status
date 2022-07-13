#pragma once

#include <string>

class system_impl
{
public:
	virtual bool start(); // Returns true if succesfull
	virtual void stop();
	virtual std::wstring get_property(const std::string&, const std::wstring&, int);
};

class system_info
{
public:
	system_info();
	~system_info();

	void fetch_specs();
	void update_info();

	// Constants
	std::string cpu_name;
	std::string gpu_name;

	// Variables

	// Acpi_ThermalZoneTemperature, in Celsius
	float temperature;
private:
	// TODO: unique_ptr
	system_impl* m_impl;
};