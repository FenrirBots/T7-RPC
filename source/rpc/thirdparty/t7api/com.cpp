#include "thirdparty/t7api/t7api.h"

int(__fastcall* t7api::com::gameinfo::getmapidforname)(const char*) = 
    (int(__fastcall*)(const char*))((unsigned long long)GetModuleHandle(0) + (0x7FF773A039D0 - 0x7FF771910000));
//    reinterpret_cast<ulong(__fastcall*)(const char*)>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A041C0 - 0x7FF771910000));

bool(__cdecl* t7api::com::isingame)() = 
    reinterpret_cast<bool(__cdecl*)()>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A58D20 - 0x7FF771910000));

bool(__fastcall* t7api::com::ismode)(eModes) = 
    reinterpret_cast<bool(__fastcall*)(eModes)>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A07DD0 - 0x7FF771910000));

int(__cdecl* t7api::com::sessionmode::getmode)() = 
    reinterpret_cast<int(__cdecl*)()>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A07790 - 0x7FF771910000));

// Wouldnt accept reinterpret cast when testing
long long(__cdecl* t7api::com::localclient::getcontrollerindex)(int) = 
    (long long(__cdecl*)(int))((unsigned long long)GetModuleHandle(0) + (0x7FF773A00390 - 0x7FF771910000));
    
long long (__cdecl* t7api::com::controllerindexes::getprimary)() =
    (long long(__cdecl*)())((unsigned long long)GetModuleHandle(0) + (0x7FF773A002E0 - 0x7FF771910000));

bool(__cdecl* t7api::com::sessionmode::isonlinegame)() = 
    reinterpret_cast<bool(__cdecl*)()>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A07E10 - 0x7FF771910000));
//
//    reinterpret_cast<long long(__cdecl*)(int)>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF773A00390- 0x7FF771910000));