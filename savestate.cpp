// savestate.cpp

#include <iostream>
#include "Windows.h"
#include "process.h"
#include <vector>
#include <TlHelp32.h>

using namespace std;

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); i++)
    {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];

        ///char hex_string[20];
        ///sprintf_s(hex_string, "%X", addr);
        ///std::cout << hex_string << std::endl;
    }
    return addr;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

int main()
{
    AllocConsole();
    FILE* f = new FILE;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitle(L"savestate");
    std::cout << "console test\n" << std::endl;

    std::cout << "F1 - save state\nF3 - load state" << std::endl;


    HWND hwnd = FindWindow(0, L"The End is Nigh");
    DWORD procId = 0;
    GetWindowThreadProcessId(hwnd, &procId);

    //std::cout << procId << std::endl;

    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"TheEndIsNigh.exe");

    //std::cout << moduleBase << std::endl;
    //std::cout << moduleBase + 0x2D8AEC << std::endl;
    //char hex_string[20];
    //sprintf_s(hex_string, "%X", moduleBase);
    //std::cout << hex_string << std::endl;



    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);


    double xpos = 0;
    double ypos = 0;
    double xspeed = 0;
    double yspeed = 0;
    double playerdirection = 0;

    int savedxmap = 0;
    int savedymap = 0;

    //int filetime = 0;
    bool statesaved = false;

    int currentxmap = 0;
    int currentymap = 0;

    while (true)
    {
        uintptr_t xposaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x74,0x20,0x1C,0x54,0x0,0x18,0x28 });
        uintptr_t yposaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x74,0x20,0x1C,0x54,0x0,0x18,0x30 });

        uintptr_t xspeedaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x74,0x20,0x1C,0x0,0x34C,0x174,0x98 });
        uintptr_t yspeedaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x74,0x20,0x1C,0x0,0x34C,0x174,0xA0 });

        uintptr_t playerdirectionaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x30,0x14,0x1C,0x74,0x20,0x1C,0x2C4,0x0,0x6B0 });

        uintptr_t xmapaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x74,0x20,0x24,0x200,0x0,0x178 });
        uintptr_t ymapaddr = FindDMAAddy(hProcess, moduleBase + 0x2D8AEC, { 0x2C,0x18,0x10,0x80,0x20,0x24,0x200,0x0,0x17C });

        uintptr_t filetimeaddr = moduleBase + 0x2C67E0;

        ReadProcessMemory(hProcess, (BYTE*)xmapaddr, &currentxmap, 4, 0);
        ReadProcessMemory(hProcess, (BYTE*)ymapaddr, &currentymap, 4, 0);

        //std::cout << '(' << currentxmap << ',' << currentymap << std::endl;
        //Sleep(1000);

        //std::cout << sizeof(xposaddr) << std::endl;
        //std::cout << sizeof(yposaddr) << std::endl;
        

        //int h = filetime / 216000;
        //int m = (filetime - h * 216000) / 3600;
        //float s = (filetime - h * 216000 - m * 3600) / float(60);




        if (GetAsyncKeyState(VK_F1)) 
        {

            statesaved = true;

            ReadProcessMemory(hProcess, (BYTE*)xposaddr, &xpos, 8, 0);
            ReadProcessMemory(hProcess, (BYTE*)yposaddr, &ypos, 8, 0);
            ReadProcessMemory(hProcess, (BYTE*)xspeedaddr, &xspeed, 8, 0);
            ReadProcessMemory(hProcess, (BYTE*)yspeedaddr, &yspeed, 8, 0);
            ReadProcessMemory(hProcess, (BYTE*)playerdirectionaddr, &playerdirection, 8, 0);
            ReadProcessMemory(hProcess, (BYTE*)xmapaddr, &savedxmap, 4, 0);
            ReadProcessMemory(hProcess, (BYTE*)ymapaddr, &savedymap, 4, 0);

            //std::cout << savedxmap << ',' << savedymap << std::endl;

            std::cout << "saved" << std::endl;
            Sleep(500);


            /*cout << fixed;
            cout.precision(3);

            std::cout << "time: " << h << ":" << m << ":" << s << " (" << filetime << ")" << std::endl;
            std::cout << "map_pos: " << xmap << ", " << ymap << std::endl;

            cout.precision(20);
            std::cout << "pos: " << xpos << ", " << ypos << std::endl;
            std::cout << "speed: " << xspeed << ", " << yspeed << std::endl;

            if (playerdirection == 1) {
                std::cout << "right" << std::endl;
            }
            else if (playerdirection == -1) {
                std::cout << "left" << std::endl;
            }*/

            
        }
        if (GetAsyncKeyState(VK_F3))
        {
            if (statesaved==true) {
                //std::cout << savedxmap << ',' << savedymap << std::endl;

                if (currentxmap == savedxmap and currentymap == savedymap)
                {

                    WriteProcessMemory(hProcess, (BYTE*)playerdirectionaddr, &playerdirection, 8, 0);
                    WriteProcessMemory(hProcess, (BYTE*)xposaddr, &xpos, 8, 0);
                    WriteProcessMemory(hProcess, (BYTE*)yposaddr, &ypos, 8, 0);
                    WriteProcessMemory(hProcess, (BYTE*)xspeedaddr, &xspeed, 8, 0);
                    WriteProcessMemory(hProcess, (BYTE*)yspeedaddr, &yspeed, 8, 0);
                }
                else
                {

                    WriteProcessMemory(hProcess, (BYTE*)xmapaddr, &savedxmap, 4, 0);
                    WriteProcessMemory(hProcess, (BYTE*)ymapaddr, &savedymap, 4, 0);


                }

                /*cout.precision(20);
                std::cout << "pos: " << xpos << ", " << ypos << std::endl;
                std::cout << "speed: " << xspeed << ", " << yspeed << std::endl;

                if (playerdirection == 1) {
                    std::cout << "right" << std::endl;
                }
                else if (playerdirection == -1) {
                    std::cout << "left" << std::endl;
                }
                */

                std::cout << "loaded" << std::endl;
            }
            else std::cout << "no saved state" << std::endl;
           
        }
    }
}

