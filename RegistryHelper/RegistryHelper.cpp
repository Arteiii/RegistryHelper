#include "RegistryHelper.h"

// Constructor initializes the error code to ERROR_SUCCESS
RegistryHelper::RegistryHelper()
  : m_errorCode(ERROR_SUCCESS)
{
}

// Read a DWORD value from the registry. This function is marked as noexcept
// to indicate that it does not throw exceptions during its execution.
DWORD
RegistryHelper::RegGetDword(HKEY hKey,
                            const std::wstring& subKey,
                            const std::wstring& value) noexcept
{
  DWORD data{};
  DWORD dataSize = sizeof(data);

  // Use  specifier to indicate that the function doesn't throw
  // exceptions
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_DWORD,
                              nullptr,
                              &data,
                              &dataSize);

  if (m_errorCode != ERROR_SUCCESS) {
    // Throw a RegistryError exception if an error occurs
    throw RegistryError("Cannot read DWORD from registry.", m_errorCode);
  }

  return data;
}

// Read a string value from the registry
std::wstring
RegistryHelper::RegGetString(HKEY hKey,
                             const std::wstring& subKey,
                             const std::wstring& value) noexcept
{
  DWORD dataSize{};
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_SZ,
                              nullptr,
                              nullptr,
                              &dataSize);

  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read string from registry", m_errorCode);
  }

  // Use smart pointers for automatic memory management
  std::vector<wchar_t> data(dataSize / sizeof(wchar_t));

  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_SZ,
                              nullptr,
                              data.data(),
                              &dataSize);

  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read string from registry", m_errorCode);
  }

  // Resize the vector based on the actual size of the string
  data.resize(dataSize / sizeof(wchar_t) - 1);

  return std::wstring(data.begin(), data.end());
}

// Read a multi-string value from the registry
std::vector<std::wstring>
RegistryHelper::RegGetMultiString(HKEY hKey,
                                  const std::wstring& subKey,
                                  const std::wstring& value) noexcept
{
  DWORD dataSize{};
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_MULTI_SZ,
                              nullptr,
                              nullptr,
                              &dataSize);

  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read multi-string from registry", m_errorCode);
  }

  // Use smart pointers for automatic memory management
  std::vector<wchar_t> data(dataSize / sizeof(wchar_t));

  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_MULTI_SZ,
                              nullptr,
                              data.data(),
                              &dataSize);

  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read multi-string from registry", m_errorCode);
  }

  // Resize the vector based on the actual size of the multi-string
  data.resize(dataSize / sizeof(wchar_t));

  // Parse the double-NUL-terminated string into a vector<wstring>
  std::vector<std::wstring> result;
  const wchar_t* currStringPtr = data.data();
  while (*currStringPtr != L'\0') {
    const size_t currStringLength = wcslen(currStringPtr);
    result.push_back(std::wstring{ currStringPtr, currStringLength });
    currStringPtr += currStringLength + 1;
  }

  return result;
}
