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
