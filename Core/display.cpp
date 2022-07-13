#include "display.hpp"

#ifdef _WIN32
#include "display_win.hpp"
#endif

#include <new>
#include <math.h>

//===================================================
// PLACEHOLDER IMPLEMENTATION
void display_impl::writeln(const std::string& txt){}
void display_impl::clear(){}
//===================================================

display::display()
{
#ifdef _WIN32
	this->m_impl = new display_win;
	static_cast<display_win*>(this->m_impl)->init();
	static_cast<display_win*>(this->m_impl)->check_size();
	this->slider_length = (int)round(static_cast<display_win*>(this->m_impl)->colums / 2);
#else
	this->m_impl = NULL;
#endif
}

display::~display() {}

void display::put_string(const std::string& name, const std::string& value)
{
	std::string txt = name + ": " + value + "\n";
	this->objects[name] = txt;
}

void display::put_int(const std::string& name, const int& value)
{
	std::string txt = std::to_string(value);
	this->objects[name] = txt;
}

void display::put_slider(const std::string& name, const int& value, const int& min, const int& max)
{
	this->put_slider(name, value, min, max, "");
}

void display::put_slider(const std::string& name, const int& value, const int& min, const int& max, const std::string& label)
{
	std::string txt = name + ":";

	if (!label.empty())
	{
		txt += "(" + label + ")";
	}

	double full_chars = (((double)value - (double)min) / (double)max) * this->slider_length;
	full_chars = round(full_chars);

	txt += "[";

	for (int i = 0; i < this->slider_length; i++)
	{
		if (i <= full_chars)
		{
			txt += "#";
		}
		else
		{
			txt += ".";
		}
	}

	txt += "]";

	this->objects[name] = txt;
	//static_cast<display_win*>(this->m_impl)->writeln(std::to_string(full_chars));
}

void display::update() 
{
#ifdef _WIN32
	static_cast<display_win*>(this->m_impl)->clear();
	static_cast<display_win*>(this->m_impl)->check_size();

	this->slider_length = (int)round(static_cast<display_win*>(this->m_impl)->colums / 2);
#endif

	auto iter = this->objects.begin();

	while (iter != this->objects.end()) {
#ifdef _WIN32
		static_cast<display_win*>(this->m_impl)->writeln(iter->second);
#endif
		++iter;
	}
}