#pragma once

struct pModule {
	std::string name;
	HMODULE address;
};

class memory_tools
{
	HANDLE gmod_process;

public:

	memory_tools() {
		gmod_process = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, GetCurrentProcessId());
	}
	~memory_tools() {
		CloseHandle(gmod_process);
	}

	std::vector<pModule> GetModules();

};



