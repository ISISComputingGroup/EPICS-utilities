#include <windows.h>
#include <imagehlp.h>
#include <iostream>
#include <cstdlib>

// Set the process affinity mask in the Windows executable image header
// Arguments: filename mask

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: file mask" << std::endl;
        return 1;
    }
    const char* file = argv[1];
    DWORD mask = atoll(argv[2]);
    LOADED_IMAGE li;
    if (MapAndLoad(file, "\\nosearch", &li, 0, 0) == 0)
    {
        std::cerr << "MapAndLoad: error " << GetLastError() << std::endl;
        return 1;
    }
    ULONG lcds;
    PIMAGE_LOAD_CONFIG_DIRECTORY plcd = (PIMAGE_LOAD_CONFIG_DIRECTORY)ImageDirectoryEntryToDataEx(
        li.MappedAddress,
        FALSE,
        IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG,
        &lcds,
        NULL
        );
    if (!plcd)
    {
      std::cerr << "ImageDirectoryEntryToDataEx: error " << GetLastError() << std::endl;
      return 1;
    }
    if (lcds != plcd->Size)
    {
      std::cerr << "ImageDirectoryEntryToDataEx: size error " << lcds << "!=" << plcd->Size << std::endl;
      return 1;
    }
    std::cout << "Old ProcessAffinityMask: " << plcd->ProcessAffinityMask << std::endl;
    plcd->ProcessAffinityMask = mask;
    std::cout << "New ProcessAffinityMask: " << plcd->ProcessAffinityMask << std::endl;
    if (UnMapAndLoad(&li) == 0)
    {
        std::cerr << "UnMapAndLoad: error " << GetLastError() << std::endl;
        return 1;
    }
    return 0;
}
