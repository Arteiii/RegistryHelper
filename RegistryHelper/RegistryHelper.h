#pragma once

#include <Windows.h>
#include <stdexcept>
#include <string>
#include <vector>

class RegistryError : public std::runtime_error {
public:
  RegistryError(const char *message, LONG errorCode)
      : std::runtime_error{message}, m_errorCode{errorCode} {}

  LONG ErrorCode() const noexcept { return m_errorCode; }

private:
  LONG m_errorCode;
};

class RegistryHelper {
public:
  RegistryHelper();

  DWORD RegGetDword(HKEY hKey, const std::wstring &subKey,
                    const std::wstring &value);

  std::wstring RegGetString(HKEY hKey, const std::wstring &subKey,
                            const std::wstring &value);

  std::vector<std::wstring> RegGetMultiString(HKEY hKey,
                                              const std::wstring &subKey,
                                              const std::wstring &value);

  std::vector<std::pair<std::wstring, DWORD>>
  RegEnumSubKeys(HKEY hKey, const std::wstring &subKey);

  std::vector<std::pair<std::wstring, DWORD>>
  RegEnumValues(HKEY hKey, const std::wstring &subKey);

  void RegSetDword(HKEY hKey, const std::wstring &subKey,
                   const std::wstring &value, DWORD data);

  void RegSetString(HKEY hKey, const std::wstring &subKey,
                    const std::wstring &value, const std::wstring &data);

  void RegSetMultiString(HKEY hKey, const std::wstring &subKey,
                         const std::wstring &value,
                         const std::vector<std::wstring> &data);

private:
  LONG m_errorCode; // store the last error code
};
