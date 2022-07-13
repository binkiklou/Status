#include "system.hpp"
#include "display.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	display d;

	int elapsed_ticks = 0;

	while (true) {
		d.put_slider("elapsed", elapsed_ticks, 0, 20, std::to_string(elapsed_ticks) + "s");
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		d.update();

		elapsed_ticks++;
	}
	return 0;
}