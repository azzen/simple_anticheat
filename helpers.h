#pragma once
#include <atlstr.h>

static void convert_wstring_char(TCHAR* input, char* outputString)
{
	size_t i;
	wcstombs_s(&i, outputString, MAX_PATH, input, wcslen(input) + 1);
}

static std::string convert_wstring_to_std(TCHAR* input) {
	std::wstring in(input);
#pragma warning(push)
	// Disable warning because of unicode -> ascii conversion
#pragma warning(disable: 4244) 
	std::string out = std::string(in.begin(), in.end());
#pragma warning(pop)
	return out;
}

static std::string convert_LPCSTR_to_std(LPCWSTR input) {
	return (std::string)(CW2A(input));
}