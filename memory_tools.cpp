#include "pch.h"
#include "memory_tools.h"
#include "helpers.h"


std::vector<pModule> memory_tools::GetModules()
{
    std::vector<pModule> vec{};
    HMODULE hMods[1024];
    HANDLE hProcess = nullptr;
    DWORD cbNeeded;
    unsigned int i;

    if (EnumProcessModules(gmod_process, hMods, sizeof(hMods), &cbNeeded))
    {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];

            if (GetModuleFileNameEx(gmod_process, hMods[i], szModName,
                sizeof(szModName) / sizeof(TCHAR)))
            {
                vec.push_back(pModule{ convert_wstring_to_std(szModName), hMods[i] });
            }
        }
    }

    return vec;
}
