#include <iostream>
#include <windows.h> // Required for memory manipulation
#include <thread>
#include <chrono>

DWORD getModuleBaseAddress(DWORD processId, const char* moduleName) {
    DWORD baseAddress = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(snapshot, &moduleEntry)) {
            do {
                if (_stricmp(moduleEntry.szModule, moduleName) == 0) {
                    baseAddress = (DWORD)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &moduleEntry));
        }
        CloseHandle(snapshot);
    }
    return baseAddress;
}

DWORD dmaAddr(HANDLE processHandle, DWORD base, int* offsets, int offsetCount) {
    DWORD addr = base;
    for (int i = 0; i < offsetCount; ++i) {
        ReadProcessMemory(processHandle, (LPCVOID)addr, &addr, sizeof(addr), NULL);
        addr += offsets[i];
    }
    return addr;
}

int main() {
    HWND windowHandle = FindWindowA(NULL, "Among Us");
    if (windowHandle == NULL) {
        std::cout << "Among Us window not found!\n";
        return 1;
    }

    DWORD processId;
    GetWindowThreadProcessId(windowHandle, &processId);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    const char* moduleName = "GameAssembly.dll";
    DWORD baseAddress = getModuleBaseAddress(processId, moduleName);

    float speedVal = 10.0;
    int imposterVal = 1;
    int ghostVal = 1;

    while (true) {
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            DWORD addr = dmaAddr(processHandle, baseAddress + 0x0144BB70, new int[2]{ 0x5C, 0x4 }, 2);
            WriteProcessMemory(processHandle, (LPVOID)(addr + 0x14), &speedVal, sizeof(speedVal), NULL);

            if (speedVal == 10.0)
                speedVal = 1.0;
            else
                speedVal = 10.0;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            DWORD addr = dmaAddr(processHandle, baseAddress + 0x0144BB70, new int[3]{ 0x5C, 0x0, 0x34 }, 3);
            WriteProcessMemory(processHandle, (LPVOID)(addr + 0x28), &imposterVal, sizeof(imposterVal), NULL);

            if (imposterVal == 1)
                imposterVal = 0;
            else
                imposterVal = 1;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (GetAsyncKeyState(VK_F3) & 0x8000) {
            DWORD addr = dmaAddr(processHandle, baseAddress + 0x0144BB70, new int[3]{ 0x5C, 0x0, 0x34 }, 3);
            WriteProcessMemory(processHandle, (LPVOID)(addr + 0x29), &ghostVal, sizeof(ghostVal), NULL);

            if (ghostVal == 1)
                ghostVal = 0;
            else
                ghostVal = 1;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    CloseHandle(processHandle);
    return 0;
}
