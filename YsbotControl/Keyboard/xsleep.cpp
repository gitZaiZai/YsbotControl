#include "stdafx.h"
#include "xsleep.h"

struct XSleep_Structure {
	int duration;
	HANDLE event_handle;
};

DWORD WINAPI XSleepThread(LPVOID wait_time) {
	XSleep_Structure *sleep = (XSleep_Structure *)wait_time;
	Sleep(sleep->duration);
	SetEvent(sleep->event_handle);
	return 0;
}

void XSleep(int millisecond) {
	XSleep_Structure sleep;

	sleep.duration = millisecond;
	sleep.event_handle = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD thread_id;
	CreateThread(NULL, 0, &XSleepThread, &sleep, 0, &thread_id);

	MSG msg;

	while (::WaitForSingleObject(sleep.event_handle, 0) == WAIT_TIMEOUT) {
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	CloseHandle(sleep.event_handle);
}