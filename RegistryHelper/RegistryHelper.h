#pragma once

#include <Windows.h>
#include <stdexcept>
#include <string>
#include <vector>

class RegistryError : public std::runtime_error
{
public:
  RegistryError(const char* message, LONG errorCode)
    : std::runtime_error{ message }
    , m_errorCode{ errorCode }
  {
  }

  LONG ErrorCode() const noexcept { return m_errorCode; }

private:
  LONG m_errorCode;
};

class RegistryHelper
{
public:
  RegistryHelper();

  DWORD RegGetDword(HKEY hKey,
                    const std::wstring& subKey,
                    const std::wstring& value) noexcept;
  std::wstring RegGetString(HKEY hKey,
                            const std::wstring& subKey,
                            const std::wstring& value) noexcept;
  std::vector<std::wstring> RegGetMultiString(
    HKEY hKey,
    const std::wstring& subKey,
    const std::wstring& value) noexcept;

private:
  LONG m_errorCode; // store the last error code
};
