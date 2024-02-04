# RegistryHelper - Accessing the Windows Registry with Modern C++

This project provides a `RegistryHelper` class for accessing the Windows Registry using Modern C++. The implementation is based on the Microsoft Dev Blog post "[Use Modern C++ to Access the Windows Registry](https://learn.microsoft.com/en-us/archive/msdn-magazine/authors/giovanni_dicanio)" by [Giovanni Dicanio](https://learn.microsoft.com/en-us/archive/msdn-magazine/2017/may/%5Carchive%5Cmsdn-magazine%5Cauthors%5CGiovanni_Dicanio).

## Overview

The `RegistryHelper` class allows you to read DWORD, string, and multi-string values from the Windows Registry in a convenient and modern C++ manner.

## Usage

To use the `RegistryHelper` class in your project, follow these steps:

1. Include the "RegistryHelper.h" header file in your project.
2. Add the "RegistryHelper.cpp" source file to your project.

### Example Usage

```cpp
#include "RegistryHelper.h"
#include <iostream>

int
main()
{
  try {
    RegistryHelper registryHelper;
    std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";
    HKEY hKey = HKEY_LOCAL_MACHINE;

    // Example 1: Reading DWORD value (REG_DWORD)
    std::wstring valueNameDword = L"BootDriverFlags";

    DWORD dwordValue = registryHelper.RegGetDword(hKey, subKey, valueNameDword);
    std::wcout << L"DWORD Value: " << dwordValue << std::endl;

    // Example 2: Reading String value (REG_SZ)
    std::wstring subKeyString = L"SYSTEM\\CurrentControlSet\\Control";
    std::wstring valueNameString = L"CurrentUser";

    std::wstring stringValue =
      registryHelper.RegGetString(hKey, subKey, valueNameString);
    std::wcout << L"String Value: " << stringValue << std::endl;

    // Example 3: Reading Multi-String value (REG_MULTI_SZ)
    std::wstring valueNameMultiString = L"PreshutdownOrder";

    std::vector<std::wstring> multiStringValue =
      registryHelper.RegGetMultiString(hKey, subKey, valueNameMultiString);

    std::wcout << L"Multi-String Values: " << std::endl;
    for (const auto& str : multiStringValue) {
      std::wcout << L"  " << str << std::endl;
    }
  } catch (const RegistryError& ex) {
    std::cerr << "Registry Error: " << ex.what()
              << " (Error Code: " << ex.ErrorCode() << ")" << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
  }

  return 0;
}
```

Adjust the registry keys, subkeys, and value names based on your specific use case.

## Credits

This project is inspired by the Microsoft Dev Blog post "Use Modern C++ to Access the Windows Registry" by Giovanni Dicanio. The original blog post can be found [here](https://learn.microsoft.com/en-us/archive/msdn-magazine/2017/may/c-use-modern-c-to-access-the-windows-registry).

## License

This project is provided under the MIT License. See the LICENSE file for details.
