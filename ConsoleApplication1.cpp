#include <stdio.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <thread>
#include <string>

#include "Volumizer.h"

Seltrad::Engine::Volumizer MAIN;
float Mouse_Sense = 0.001;
float KB_Sence = 0.1;

void inputeventer(bool* Running) {
	POINT
		cur_point  = { 0,0 },
		last_point = { 0,0 };

	float f, s;
	bool Occup = true;
	while (true) {
		f = 0;
		s = 0;
		if (Occup)
		{
			GetCursorPos(&cur_point);
			SetCursorPos(500, 500);
			Sleep(10);
			MAIN.MainPlayer.Turn(-(cur_point.x - 500) * Mouse_Sense, (cur_point.y - 500) * Mouse_Sense);
			last_point = cur_point;
		}

		if (GetAsyncKeyState('W') & 0x8000)
			f += KB_Sence;
		if (GetAsyncKeyState('S') & 0x8000)
			f -= KB_Sence;
		if (GetAsyncKeyState('A') & 0x8000)
			s -= KB_Sence;
		if (GetAsyncKeyState('D') & 0x8000)
			s += KB_Sence;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
			Occup = !Occup;
		if (GetAsyncKeyState(VK_F4) & 0x1)
			*Running = false;
		MAIN.MainPlayer.Move(f, s);
		Sleep(10);
	}
}

void SetCursorVisible(bool visible) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CCI;

	GetConsoleCursorInfo(consoleHandle, &CCI);
	CCI.bVisible = visible;
	SetConsoleCursorInfo(consoleHandle, &CCI);
}

wchar_t GetBrightChar(float _V) {
	wchar_t Res = L' ';
	if (_V >= 0.95)
		Res = L'█';
	else if (_V >= 0.65)
		Res = L'▓';
	else if (_V >= 0.35)
		Res = L'▒';
	else if (_V >= 0.05)
		Res = L'░';
	return Res;
}

void SetFontSize(HANDLE h, Seltrad::INT::uint8t Height) {
	CONSOLE_FONT_INFOEX CFI = {};
	CFI.cbSize = sizeof(CFI);
	GetCurrentConsoleFontEx(h, 0, &CFI);
	CFI.dwFontSize = { 0, Height};
	wcscpy_s(CFI.FaceName, L"Consolas");
	bool check;
	check = SetCurrentConsoleFontEx(h, 0, &CFI);
	if (!check)
		wprintf(L"%i %i\n", check, GetLastError());
}

int main()
{
	bool Running = true;
	std::thread Inputer{ inputeventer, &Running };

	HANDLE handle = GetStdHandle(-11);

	SetCursorVisible(false);

	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	_setmode(_fileno(stdout), _O_U16TEXT);

	SetFontSize(handle, 2);

	keybd_event(VK_F11, NULL, NULL, 0);

	Sleep(100);

	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(handle, &CSBI);
	COORD ScreenSize = CSBI.dwMaximumWindowSize;
	ScreenSize.X -= 2;
	ScreenSize.Y -= 2;
	COORD ImageSize = { MAIN.MainPlayer.getResX(),MAIN.MainPlayer.getResY() };
	SetConsoleScreenBufferSize(handle, ScreenSize);

	wchar_t** Screen = new wchar_t* [ScreenSize.Y];
	for (size_t y = 0; y < ScreenSize.Y; y++) {
		Screen[y] = new wchar_t[ScreenSize.X + 2];
	}
	std::wstring ganz = L"";

	clock_t end, start, Engining, Consoling;
	clock_t
		FrameStart = 0,
		FrameEnd = 0;
	float
		FPS = 0;
	while (Running) {
		FrameStart = clock();
		ganz = L"";
		start = clock();
		auto Raw = MAIN.GetBrightScheme();
		end = clock();

		Engining = end - start;

		start = clock();
		//_flushall();
		//SetConsoleCursorPosition(handle, { 0,0 });
		for (size_t Sy = 0; Sy < ScreenSize.Y; Sy++) {
			for (size_t Sx = 0; Sx < ScreenSize.X; Sx++) {
				float
					ScreenX = ScreenSize.X,
					ScreenY = ScreenSize.Y,
					ImageX = ImageSize.X,
					ImageY = ImageSize.Y;
				int ix = (int)(Sx * ImageX / ScreenX);
				int iy = (int)(Sy * ImageY / ScreenY);
				int index = iy * ImageX + ix;
				Screen[Sy][Sx] = GetBrightChar(Raw[index]);
			}
			Screen[Sy][ScreenSize.X] = '\n';
			Screen[Sy][ScreenSize.X + 1] = 0;
			ganz += Screen[Sy];
		}
		//wprintf(ganz.c_str());
		WriteConsoleW(handle, ganz.c_str(), ganz.length(), nullptr, nullptr);
		end = clock();
		Consoling = end - start;

		/*SetConsoleCursorPosition(handle, { 0,0 });
		auto View = MAIN.MainPlayer.getView();
		auto Pos = MAIN.MainPlayer.getPos();
		wprintf(
			L"|Direction:________| # |Position:_________| # |EnginePS:| _<<_ |ConsolePS:| _ |FPS:|\n"
			 "|%.2f | %.2f | %.2f| # |%.2f | %.2f | %.2f| # |   %.2f  | %.2f |   %.2f   | - |%.2f|\n",
			View.Xc, View.Yc, View.Zc,
			Pos.Xc, Pos.Yc, Pos.Zc,
			Engining / 1000.f,
			Consoling * 1.f / Engining,
			Consoling / 1000.f,
			FPS
		);*/
		FrameEnd = clock();
		FPS = 1.f / (1.f * FrameEnd - FrameStart) * 1000;
	}
	SetConsoleCursorPosition(handle, { 0,0 });
	SetFontSize(handle, 16);
	Sleep(100);
	wprintf(
		L"FPSonExit: %0.2f\n"
		L"EnginePSonExit: %i\n"
		L"ConsolePSonExit: %i\n",
		FPS, Engining, Consoling
	);
}