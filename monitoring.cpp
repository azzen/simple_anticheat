#include "pch.h"
#include "game.h"
#include "monitoring.h"
#include "config.h"
#include "helpers.h"
#include <detours.h>
#include <SubAuth.h>

HMODULE(WINAPI* True_LoadLibraryW)(LPCWSTR lpLibFileName) = LoadLibraryW;
HANDLE(WINAPI* True_CreateThread)(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId) = CreateThread;

HMODULE(WINAPI* True_GetModuleHandleA)(
	LPCSTR lpModuleName
) = GetModuleHandleA;


HMODULE WINAPI HookedLoadLibrary(LPCWSTR lpLibFileName)
{
	if (wcslen(lpLibFileName) > 0) {
		iGame->appendToQueue(1, convert_LPCSTR_to_std(lpLibFileName));
	}
	return True_LoadLibraryW(lpLibFileName);
}

HMODULE WINAPI HookedGetModuleHandleA(LPCSTR lpModuleName) {
	std::string copy(lpModuleName);
	auto resp =  True_GetModuleHandleA(lpModuleName);
	iGame->appendToQueue(3, copy);
	return resp;
}

void monitoring::hookLibrarys() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)True_LoadLibraryW, HookedLoadLibrary);
	DetourAttach(&(PVOID&)True_GetModuleHandleA, HookedGetModuleHandleA);

	LONG lError = DetourTransactionCommit();
	if (lError != NO_ERROR) {
		MessageBox(HWND_DESKTOP, L"Failed to load crucial security components", L"SimpleAC", MB_OK);
	}
}
