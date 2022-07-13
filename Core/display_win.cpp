#include "display_win.hpp"

#include <memory>
#include <iostream>

#include <Windows.h>

void display_win::init()
{
	hstd = new HANDLE;
	og_mode = new DWORD;

	*static_cast<HANDLE*>(hstd) = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD mode;

	if (!GetConsoleMode(*static_cast<HANDLE*>(hstd), &mode))
	{
		std::cout << "Couldn't get console mode\n";
		std::exit(1);
	}

	*static_cast<DWORD*>(og_mode) = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if (!SetConsoleMode(*static_cast<HANDLE*>(this->hstd), mode))
	{
		std::cout << "Couldn't set console mode\n";
		std::exit(1);
	}
}

void display_win::writeln(const std::string& txt) 
{
	std::cout << txt << '\n';
}

void display_win::clear()
{
	// https://docs.microsoft.com/en-us/windows/console/clearing-the-screen
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	HANDLE hConsole = *static_cast<HANDLE*>(this->hstd);

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}

void display_win::check_size()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(*static_cast<HANDLE*>(this->hstd), &csbi);
	this->colums = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	this->rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}