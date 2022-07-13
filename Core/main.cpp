#include "system.hpp"
#include "display.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	system_info sys;
	display d;

	int elapsed_ticks = 0;

	sys.fetch_specs();

	while (true) {
		sys.update_info();
		d.put_slider("temp", sys.temperature, 0, 100, std::to_string((int)sys.temperature) + "*C");

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		d.update();

		elapsed_ticks++;
	}
	return 0;
}