#include "RegistryHelper.h"
#include <iostream>

int main()
{
	try
	{
		RegistryHelper registryHelper;
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

		std::wstring stringValue = registryHelper.RegGetString(hKey, subKey, valueNameString);
		std::wcout << L"String Value: " << stringValue << std::endl;

		//
		// Example 3: Reading Multi-String value (REG_MULTI_SZ)
		//
		std::wstring valueNameMultiString = L"PreshutdownOrder";

		std::vector<std::wstring> multiStringValue =
			registryHelper.RegGetMultiString(hKey, subKey, valueNameMultiString);

		std::wcout << L"Multi-String Values: " << std::endl;
		for (const auto& str : multiStringValue)
		{
			std::wcout << L"  " << str << std::endl;
		}

		//
		// Example 4: Enumerate sub-keys
		//
		std::vector<std::pair<std::wstring, DWORD>> subKeys = registryHelper.RegEnumSubKeys(hKey, subKey);

		std::wcout << L"Sub-Keys: " << std::endl;
		for (const auto& subKeyPair : subKeys)
		{
			std::wcout << L"  " << subKeyPair.first << std::endl;
		}

		//
		// Example 5: Enumerate values
		//
		std::vector<std::pair<std::wstring, DWORD>> values = registryHelper.RegEnumValues(hKey, subKey);

		std::wcout << L"Values: " << std::endl;
		for (const auto& valuePair : values)
		{
			std::wcout << L"  " << valuePair.first << L" (Type: " << valuePair.second << L")" << std::endl;
		}
	}
	catch (const RegistryError& ex)
	{
		std::cerr << "Registry Error: " << ex.what() << " (Error Code: " << ex.ErrorCode() << ")" << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}

	return 0;
}
