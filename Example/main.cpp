#include "pch.h"

#include "RegistryInterface.hpp"

int
main()
{
  try {
    RegistryHelper registryHelper;
    RegistryInterface registryInterface;
    std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";
    HKEY hKey = HKEY_LOCAL_MACHINE;

    //
    // Example 1: Reading DWORD value (REG_DWORD)
    //
    std::wstring valueNameDword = L"BootDriverFlags";

    DWORD dwordValue = registryHelper.RegGetDword(hKey, subKey, valueNameDword);
    std::wcout << L"DWORD Value: " << dwordValue << std::endl;

    //
    // Example 2: Reading String value (REG_SZ)
    //
    std::wstring valueNameString = L"CurrentUser";

    std::wstring stringValue =
      registryHelper.RegGetString(hKey, subKey, valueNameString);
    std::wcout << L"String Value: " << stringValue << std::endl;

    //
    // Example 3: Reading Multi-String value (REG_MULTI_SZ)
    //
    std::wstring valueNameMultiString = L"PreshutdownOrder";

    std::vector<std::wstring> multiStringValue =
      registryHelper.RegGetMultiString(hKey, subKey, valueNameMultiString);

    std::wcout << L"Multi-String Values: " << std::endl;
    for (const auto& str : multiStringValue) {
      std::wcout << L"  " << str << std::endl;
    }

    //
    // Example 4: Enumerate sub-keys
    //
    std::vector<std::pair<std::wstring, DWORD>> subKeys =
      registryHelper.RegEnumSubKeys(hKey, subKey);

    std::wcout << L"Sub-Keys: " << std::endl;
    for (const auto& subKeyPair : subKeys) {
      std::wcout << L"  " << subKeyPair.first << std::endl;
    }

    //
    // Example 5: Enumerate values
    //
    std::vector<std::pair<std::wstring, DWORD>> values =
      registryHelper.RegEnumValues(hKey, subKey);

    std::wcout << L"Values: " << std::endl;
    for (const auto& valuePair : values) {
      std::wcout << L"  " << valuePair.first << L" (Type: "
                 << registryInterface.GetDataTypeName(valuePair.second) << L")"
                 << std::endl;
    }

    // Example 6: Enumerate and display values under CurrentVersion
    std::vector<RegistryKeyValue> currentUserValues =
      registryInterface.EnumerateCurrentUserValues();

    if (currentUserValues.empty()) {
      std::wcout << L"No values found in the specified registry key."
                 << std::endl;
    } else {
      // Display the results
      std::wcout << L"Current User Registry Values:" << std::endl;
      for (const auto& subKeyInfo : currentUserValues) {
        std::wcout << L"Subkey Name: " << subKeyInfo.name << std::endl;
        std::wcout << L"Key Name: " << subKeyInfo.keyName << std::endl;

        if (!subKeyInfo.values.empty()) {
          std::wcout << L"Values:" << std::endl;
          for (const auto& value : subKeyInfo.values) {
            std::wcout << L"  Value Name: " << value.name << std::endl;
            std::wcout << L"    Data Type: " << value.dataTypeName << std::endl;
            std::wcout << L"    String Value: " << value.stringValue
                       << std::endl;
            std::wcout << L"    DWORD Value: " << value.dwordValue << std::endl;
          }
        } else {
          std::wcout << L"No values found for this subkey." << std::endl;
        }

        std::wcout << std::endl;
      }
    }
  } catch (const RegistryError& ex) {
    std::cerr << "Registry Error: " << ex.what()
              << " (Error Code: " << ex.GetErrorCode() << ")" << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
  }

  return 0;
}
