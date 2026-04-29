#include <stdio.h>
#include <Windows.h>
#include <thread>

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
	while (*Running) {
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

using namespace Seltrad::INT;

void SetCursorVisible(HANDLE _V, bool _Visible) {
	CONSOLE_CURSOR_INFO CCI;

	GetConsoleCursorInfo(_V, &CCI);
	CCI.bVisible = _Visible;
	SetConsoleCursorInfo(_V, &CCI);
}

void SetFontSize(HANDLE _H, uint8t _Height) {
	CONSOLE_FONT_INFOEX CFI = {};
	CFI.cbSize = sizeof(CFI);
	GetCurrentConsoleFontEx(_H, 0, &CFI);
	CFI.dwFontSize = { 0, _Height};
	wcscpy_s(CFI.FaceName, L"Consolas");
	bool check;
	SetCurrentConsoleFontEx(_H, 0, &CFI);
}

void SetCursorVisibleM(HANDLE* _Hs, uint8t _Amount, bool _Visible) {
	for (uint8t i = 0; i < _Amount; i++) {
		SetCursorVisible(_Hs[i], _Visible);
	}
}

void SetFontSizeM(HANDLE* _Hs, uint8t _Amount, uint8t _Height) {
	for (uint8t i = 0; i < _Amount; i++) {
		SetFontSize(_Hs[i], _Height);
	}
}

void UpdateHM(HANDLE* _Hs) {
	SetConsoleActiveScreenBuffer(_Hs[0]);
}

void UpdateCurrentHandle(HANDLE* _Hs, uint8t _Amount, wchar_t* _Screen, uint32t _ScreenSize, bool* _Running, clock_t* _Time) {
	{
		clock_t start, end;
		while (*_Running)
		{
			start = clock();

			HANDLE* Temp = new HANDLE[_Amount];
			for (uint8t i = 1; i < _Amount; i++) {
				Temp[i - 1] = _Hs[i];
			}
			Temp[_Amount - 1] = _Hs[0];
			uint64lt written;
			WriteConsoleOutputCharacterW(_Hs[0], _Screen, _ScreenSize, { 0,0 }, &written);
			_Hs = Temp;
			Sleep(5);

			end = clock();
			*_Time = end - start;
		}
	}
}

int main()
{
	bool Running = true;
	std::thread Inputer{ inputeventer, &Running };

	uint8t HandleAmount = 3;
	HANDLE handle = GetStdHandle(-11);
	HANDLE* HandleArr = new HANDLE[HandleAmount];
	HandleArr[0] = GetStdHandle(-11);
	for (uint8t i = 1; i < HandleAmount; i++)
		HandleArr[i] = CreateConsoleScreenBuffer(GENERIC_ALL, 0, 0, CONSOLE_TEXTMODE_BUFFER, 0);

	SetCursorVisibleM(HandleArr, HandleAmount, false);

	SetFontSizeM(HandleArr, HandleAmount, 2);

	keybd_event(VK_F11, NULL, NULL, 0);

	Sleep(100);

	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(handle, &CSBI);
	COORD ScreenSize = CSBI.dwMaximumWindowSize;
	ScreenSize.X -= 0;
	ScreenSize.Y -= 0;
	COORD ImageSize = { MAIN.MainPlayer.getResX(),MAIN.MainPlayer.getResY() };
	SetConsoleScreenBufferSize(handle, ScreenSize);

	wchar_t* Screen = new wchar_t[ScreenSize.Y * ScreenSize.X];

	clock_t end, start, Engining, Consoling;
	clock_t
		FrameStart = 0,
		FrameEnd = 0,
		Frame = 0;

	clock_t ThreadFrame;
	std::thread Updater{ [&HandleArr, HandleAmount, &Screen, &ScreenSize, &Running, &ThreadFrame]() {
		UpdateCurrentHandle(HandleArr, HandleAmount, Screen, ScreenSize.X * ScreenSize.Y, &Running, &ThreadFrame);
		} };

	while (Running) {
		FrameStart = clock();
		start = clock();
		auto Raw = MAIN.GetBrightScheme();
		end = clock();

		Engining = end - start;

		start = clock();
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
				Screen[Sy * ScreenSize.X + Sx] = GetBrightChar(Raw[index]);
			}
		}
		UpdateHM(HandleArr);
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
		Frame = FrameEnd - FrameStart;
	}

	Inputer.join();
	Updater.join();


	SetConsoleCursorPosition(GetStdHandle(-11), { 0,0 });
	SetFontSize(GetStdHandle(-11), 16);
	Sleep(100);
	wprintf(
		L"   Inner time per Frame upon Exit: %04i ms | %02.2f\n"
		L"               EnginePS upon Exit: %04i ms | %02.2f %%\n"
		L"              ConsolePS upon Exit: %04i ms | %02.2f %%\n"
		L"Thread's time per Frame upon Exit: %04i ms | %02.2f\n",
		Frame, 1.f / Frame * 1000.f,
		Engining, 100.f * Engining / Frame,
		Consoling, 100.f * Consoling / Frame,
		ThreadFrame, 1.f / ThreadFrame * 1000.f
	);
}
