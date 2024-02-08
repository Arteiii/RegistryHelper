#include "RegistryHelper.h"

#include <memory>

// Constructor initializes the error code to ERROR_SUCCESS
RegistryHelper::RegistryHelper()
  : m_errorCode(ERROR_SUCCESS)
{
}

// Read a DWORD value from the registry
DWORD
RegistryHelper::RegGetDword(HKEY hKey,
                            const std::wstring& subKey,
                            const std::wstring& value)
{
  DWORD data{};
  DWORD dataSize = sizeof(data);

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
                             const std::wstring& value)
{
  // Retrieve the size of the string value data
  DWORD dataSize{};
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_SZ,
                              nullptr,
                              nullptr,
                              &dataSize);

  // Check if the value information retrieval was successful
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read string from registry", m_errorCode);
  }

  // Use smart pointers for automatic memory management
  std::vector<wchar_t> data(dataSize / sizeof(wchar_t));

  // Retrieve the actual string value data
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_SZ,
                              nullptr,
                              data.data(),
                              &dataSize);

  // Check if the value data retrieval was successful
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read string from registry", m_errorCode);
  }

  // Resize the vector based on the actual size of the string
  data.resize(dataSize / sizeof(wchar_t) - 1);

  // Convert the wchar_t vector to a wstring and return
  return std::wstring(data.begin(), data.end());
}

// Read a multi-string value from the registry
std::vector<std::wstring>
RegistryHelper::RegGetMultiString(HKEY hKey,
                                  const std::wstring& subKey,
                                  const std::wstring& value)
{
  // Retrieve the size of the multi-string value data
  DWORD dataSize{};
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_MULTI_SZ,
                              nullptr,
                              nullptr,
                              &dataSize);

  // Check if the value information retrieval was successful
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read multi-string from registry", m_errorCode);
  }

  // Use smart pointers for automatic memory management
  std::vector<wchar_t> data(dataSize / sizeof(wchar_t));

  // Retrieve the actual multi-string value data
  m_errorCode = ::RegGetValue(hKey,
                              subKey.c_str(),
                              value.c_str(),
                              RRF_RT_REG_MULTI_SZ,
                              nullptr,
                              data.data(),
                              &dataSize);

  // Check if the value data retrieval was successful
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot read multi-string from registry", m_errorCode);
  }

  // Resize the vector based on the actual size of the multi-string
  data.resize(dataSize / sizeof(wchar_t));

  // Parse the double-NUL-terminated string into a vector of wstrings
  std::vector<std::wstring> result;
  const wchar_t* currStringPtr = data.data();
  while (*currStringPtr != L'\0') {
    const size_t currStringLength = wcslen(currStringPtr);
    result.push_back(std::wstring{ currStringPtr, currStringLength });
    currStringPtr += currStringLength + 1;
  }

  // Return the vector containing individual strings from the multi-string
  return result;
}

std::vector<std::pair<std::wstring, DWORD>>
RegistryHelper::RegEnumSubKeys(HKEY hKey, const std::wstring& subKey)
{
  HKEY keyHandle;

  // Open the specified key (root key or subkey)
  LONG openResult = RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &keyHandle);

  if (openResult != ERROR_SUCCESS) {
    return {}; // Return an empty vector
  }

  // Retrieve information about the specified registry key
  DWORD subKeyCount{};      // Number of subkeys under the key
  DWORD maxSubKeyNameLen{}; // Maximum length of a subkey name
  LONG retCode = ::RegQueryInfoKey(keyHandle,
                                   nullptr, // No user-defined class
                                   nullptr, // No user-defined class size
                                   nullptr, // Reserved
                                   &subKeyCount,
                                   &maxSubKeyNameLen,
                                   nullptr, // No subkey class length
                                   nullptr, // No value count
                                   nullptr, // No value max length
                                   nullptr, // No security descriptor
                                   nullptr, // No last write time
                                   nullptr);

  // Check if the key information retrieval was successful
  if (retCode != ERROR_SUCCESS) {
    // Handle error, close the key handle and return
    RegCloseKey(keyHandle);
    throw RegistryError{
      "RegQueryInfoKey failed while preparing for value enumeration", retCode
    };
  }

  // Allocate a buffer for storing subkey names
  maxSubKeyNameLen++;

  auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

  // Vector to store pairs of subkey names and types
  std::vector<std::pair<std::wstring, DWORD>> subKeys;

  // Enumerate subkeys under the registry key
  for (DWORD index = 0; index < subKeyCount; index++) {
    DWORD subKeyNameLen = maxSubKeyNameLen;

    // Retrieve information about the specified subkey
    retCode = ::RegEnumKeyEx(keyHandle,
                             index,
                             nameBuffer.get(),
                             &subKeyNameLen,
                             nullptr, // Reserved
                             nullptr, // No class information
                             nullptr, // No class size
                             nullptr  // No last write time
    );

    // Check if the subkey information retrieval was successful
    if (retCode != ERROR_SUCCESS) {
      // Close the key handle and handle the error
      RegCloseKey(keyHandle);
      throw RegistryError{ "Cannot get subkey info from the registry",
                           retCode };
    }

    // Add the subkey name and type to the vector
    subKeys.push_back(
      std::make_pair(std::wstring{ nameBuffer.get(), subKeyNameLen }, 0));
  }

  // Close the key handle
  RegCloseKey(keyHandle);

  // Return the vector containing subkey names and types
  return subKeys;
}

std::vector<std::pair<std::wstring, DWORD>>
RegistryHelper::RegEnumValues(HKEY hKey, const std::wstring& subKey)
{
  HKEY keyHandle;

  // Open the specified key (root key or subkey)
  LONG openResult = RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &keyHandle);

  if (openResult != ERROR_SUCCESS) {
    // Handle error, you might want to throw an exception or return an empty
    // vector
    return {};
  }

  DWORD valueCount{};
  DWORD maxValueNameLen{};
  LONG retCode = ::RegQueryInfoKey(keyHandle,
                                   nullptr, // no user-defined class
                                   nullptr, // no user-defined class size
                                   nullptr, // reserved
                                   nullptr, // no subkey count
                                   nullptr, // no subkey max length
                                   nullptr, // no subkey class length
                                   &valueCount,
                                   &maxValueNameLen,
                                   nullptr, // no max value length
                                   nullptr, // no security descriptor
                                   nullptr  // no last write time
  );
  if (retCode != ERROR_SUCCESS) {
    // Handle error, close the key handle and return
    RegCloseKey(keyHandle);
    throw RegistryError{
      "RegQueryInfoKey failed while preparing for value enumeration", retCode
    };
  }

  maxValueNameLen++;
  auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);
  std::vector<std::pair<std::wstring, DWORD>> valueInfo;

  // Enumerate the values
  for (DWORD index = 0; index < valueCount; index++) {
    // Get the name and type
    DWORD valueNameLen = maxValueNameLen;
    DWORD valueType{};
    retCode = ::RegEnumValue(keyHandle,
                             index,
                             nameBuffer.get(),
                             &valueNameLen,
                             nullptr, // reserved
                             &valueType,
                             nullptr, // no data
                             nullptr  // no data size
    );
    if (retCode != ERROR_SUCCESS) {
      // Handle error, close the key handle and throw an exception
      RegCloseKey(keyHandle);
      throw RegistryError{ "Cannot enumerate values: RegEnumValue failed",
                           retCode };
    }

    valueInfo.push_back(std::make_pair(
      std::wstring{ nameBuffer.get(), valueNameLen }, valueType));
  }

  // Close the key handle
  RegCloseKey(keyHandle);

  return valueInfo;
}

void
RegistryHelper::RegSetDword(HKEY hKey,
                            const std::wstring& subKey,
                            const std::wstring& value,
                            DWORD data)
{
  m_errorCode = ::RegSetKeyValue(
    hKey, subKey.c_str(), value.c_str(), REG_DWORD, &data, sizeof(data));
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot set DWORD value in registry.", m_errorCode);
  }
}

void
RegistryHelper::RegSetString(HKEY hKey,
                             const std::wstring& subKey,
                             const std::wstring& value,
                             const std::wstring& data)
{
  m_errorCode =
    ::RegSetKeyValue(hKey,
                     subKey.c_str(),
                     value.c_str(),
                     REG_SZ,
                     data.c_str(),
                     static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t)));
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot set string value in registry.", m_errorCode);
  }
}

void
RegistryHelper::RegSetMultiString(HKEY hKey,
                                  const std::wstring& subKey,
                                  const std::wstring& value,
                                  const std::vector<std::wstring>& data)
{
  // Concatenate the strings and add an extra null character at the end
  std::wstring multiString;
  for (const auto& str : data) {
    multiString += str;
    multiString.push_back(L'\0');
  }
  multiString.push_back(L'\0'); // Extra null character at the end

  m_errorCode = ::RegSetKeyValue(
    hKey,
    subKey.c_str(),
    value.c_str(),
    REG_MULTI_SZ,
    multiString.c_str(),
    static_cast<DWORD>(multiString.length() * sizeof(wchar_t)));
  if (m_errorCode != ERROR_SUCCESS) {
    throw RegistryError("Cannot set multi-string value in registry.",
                        m_errorCode);
  }
}
