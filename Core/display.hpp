#pragma once

#include <map>
#include <string>

class display_impl
{
public:
	virtual void writeln(const std::string&);
	virtual void clear();
};

class display
{
public:
	display();
	~display();

	// Dispaly a string
	void put_string(const std::string&, const std::string& );

	// Display an int
	void put_int(const  std::string&, const int&);

	// Creates a slider
	void put_slider(const std::string& name, const int& value, const int& min, const int& max);

	// Creates a slider with a label
	void put_slider(const std::string& name, const int& value, const int& min, const int& max, const std::string& label);

	// Clears and update the display
	void update();

	// Lenght of sliders(in chars)
	int slider_length = 10;
private:
	display_impl* m_impl;
	std::map<std::string, std::string> objects;
};