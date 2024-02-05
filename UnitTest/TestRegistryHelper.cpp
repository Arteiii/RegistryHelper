#include "CppUnitTest.h"
#include "RegistryHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RegistryHelperTests {
TEST_CLASS(RegistryHelperTests){
  public :

    TEST_METHOD(TestRegGetDword){ RegistryHelper registryHelper;
HKEY hKey = HKEY_LOCAL_MACHINE;
std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";
std::wstring valueName = L"BootDriverFlags";

DWORD dwordValue = registryHelper.RegGetDword(hKey, subKey, valueName);
Assert::AreNotEqual(dwordValue, static_cast<DWORD>(0));
}

TEST_METHOD(TestRegGetString)
{
  RegistryHelper registryHelper;
  HKEY hKey = HKEY_LOCAL_MACHINE;
  std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";
  std::wstring valueName = L"CurrentUser";

  std::wstring stringValue =
    registryHelper.RegGetString(hKey, subKey, valueName);
  Assert::IsFalse(stringValue.empty());
}

TEST_METHOD(TestRegGetMultiString)
{
  RegistryHelper registryHelper;
  HKEY hKey = HKEY_LOCAL_MACHINE;
  std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";
  std::wstring valueName = L"PreshutdownOrder";

  std::vector<std::wstring> multiStringValue =
    registryHelper.RegGetMultiString(hKey, subKey, valueName);
  Assert::IsFalse(multiStringValue.empty());
}

TEST_METHOD(TestRegEnumValues)
{
  RegistryHelper registryHelper;
  HKEY hKey = HKEY_LOCAL_MACHINE;
  std::wstring subKey = L"SYSTEM\\CurrentControlSet\\Control";

  try {
    auto valueInfo = registryHelper.RegEnumValues(hKey, subKey);

    Assert::IsFalse(valueInfo.empty(), L"No values were enumerated.");

    Assert::AreEqual(L"BootDriverFlags",
                     valueInfo[0].first.c_str(),
                     L"Unexpected value name.");
    Assert::AreEqual(REG_DWORD, valueInfo[0].second, L"Unexpected value type.");

  } catch (const RegistryError&) {
    Assert::Fail(L"Unexpected RegistryError exception");
  }
}

TEST_METHOD(TestRegEnumSubKeys)
{
  RegistryHelper registryHelper;
  HKEY hKey = HKEY_LOCAL_MACHINE;
  std::wstring subKey = L"SOFTWARE\\Microsoft";

  std::vector<std::pair<std::wstring, DWORD>> subKeys =
    registryHelper.RegEnumSubKeys(hKey, subKey);

  Assert::IsFalse(subKeys.empty());
  Assert::AreEqual(static_cast<DWORD>(0), subKeys[0].second);
}
}
;
}
