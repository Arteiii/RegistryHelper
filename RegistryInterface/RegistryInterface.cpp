// RegistryInterface.cpp : Defines the functions for the static library.
//
#include "pch.h"

#include "RegistryInterface.hpp"

std::wstring
RegistryInterface::GetDataTypeName(DWORD dataType) noexcept
{
  static const std::unordered_map<DWORD, std::wstring> dataTypeNames = {
    { REG_SZ, L"REG_SZ" },
    { REG_EXPAND_SZ, L"REG_EXPAND_SZ" },
    { REG_BINARY, L"REG_BINARY" },
    { REG_DWORD, L"REG_DWORD" },
  };

  auto it = dataTypeNames.find(dataType);
  return (it != dataTypeNames.end()) ? it->second : L"Unknown";
}

const wchar_t* RegistryInterface::CrashControlKey =
  L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Recovery";

// get AutoReboot
inline std::wstring
RegistryInterface::GetAutoRebootSetting()
{
  return registryHelper.RegGetString(
    HKEY_LOCAL_MACHINE, CrashControlKey, L"AutoReboot");
}

// get MiniDumpDir
inline std::wstring
RegistryInterface::GetMiniDumpDirSetting()
{
  return registryHelper.RegGetString(
    HKEY_LOCAL_MACHINE, CrashControlKey, L"MiniDumpDir");
}

RecoverySettings
RegistryInterface::GetAllSettings()
{
  RecoverySettings settings;

  // Populate settings using the private helper functions
  settings.recoveryOption = GetAutoRebootSetting();
  settings.dumpFileLocation = GetMiniDumpDirSetting();

  return settings;
}

std::vector<ServiceInfo>
RegistryInterface::EnumerateServices()
{
  std::vector<ServiceInfo> services;

  RegistryHelper registryHelper;

  // Specify the registry key for services
  const std::wstring servicesKey = L"SYSTEM\\CurrentControlSet\\Services";

  try {
    // Enumerate subkeys
    std::vector<std::pair<std::wstring, DWORD>> subKeys =
      registryHelper.RegEnumSubKeys(HKEY_LOCAL_MACHINE, servicesKey);

    // Retrieve additional information for each service
    for (const auto& subKey : subKeys) {
      ServiceInfo service;
      service.serviceName = subKey.first;

      try {
        // Retrieve the service type
        std::vector<std::pair<std::wstring, DWORD>> values =
          registryHelper.RegEnumValues(HKEY_LOCAL_MACHINE,
                                       (servicesKey + L"\\" + subKey.first));

        // Retrieve additional information for each service
        for (const auto& value : values) {
          std::wstring valueName = value.first;
          DWORD valueType = value.second;

          // Handle different value types
          RegistryKeyValue registryValue;
          registryValue.name = valueName;

          if (valueType == REG_SZ) {
            registryValue.stringValue =
              registryHelper.RegGetString(HKEY_LOCAL_MACHINE,
                                          (servicesKey + L"\\" + subKey.first),
                                          valueName);
            registryValue.dwordValue = 0; // Default value for non-DWORD types
          } else if (valueType == REG_DWORD) {
            registryValue.stringValue =
              L""; // Default value for non-STRING types
            registryValue.dwordValue =
              registryHelper.RegGetDword(HKEY_LOCAL_MACHINE,
                                         (servicesKey + L"\\" + subKey.first),
                                         valueName);
          } else {
            // Handle other value types
            registryValue.stringValue = L""; // Default value for unknown types
            registryValue.dwordValue = 0;    // Default value for unknown types
          }

          registryValue.dataTypeName = GetDataTypeName(valueType);

          // Add the value information to the service structure
          service.values.push_back(registryValue);
        }
      } catch (const std::exception& ex) {
        // Handle the exception (e.g., log or report)
        std::wcerr << L"Exception while enumerating values for service "
                   << service.serviceName << ": " << ex.what() << std::endl;
        // Continue with the next service
        continue;
      }

      services.push_back(service);
    }
  } catch (const std::exception& ex) {
    // Handle the exception (e.g., log or report)
    std::wcerr << L"Exception while enumerating services: " << ex.what()
               << std::endl;
    // Return an empty vector on exception
    return {};
  }

  return services;
}

std::vector<RegistryKeyValue>
RegistryInterface::EnumerateCurrentUserValues()
{
  std::vector<RegistryKeyValue> values;

  // Specify the registry key for CurrentUser
  const std::wstring currentUserKey =
    L"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion";

  try {
    // Enumerate values
    std::vector<std::pair<std::wstring, DWORD>> valuePairs =
      registryHelper.RegEnumValues(HKEY_CURRENT_USER, currentUserKey);

    // Retrieve additional information for each value
    for (const auto& valuePair : valuePairs) {
      RegistryKeyValue keyValue;
      keyValue.name = valuePair.first;
      DWORD valueType = valuePair.second;

      // Handle different value types
      if (valueType == REG_SZ) {
        keyValue.stringValue = registryHelper.RegGetString(
          HKEY_CURRENT_USER, currentUserKey, valuePair.first);
        keyValue.dwordValue = 0; // Default value for non-DWORD types
      } else if (valueType == REG_DWORD) {
        keyValue.stringValue = L""; // Default value for non-STRING types
        keyValue.dwordValue = registryHelper.RegGetDword(
          HKEY_CURRENT_USER, currentUserKey, valuePair.first);
      } else {
        // Handle other value types
        keyValue.stringValue = L""; // Default value for unknown types
        keyValue.dwordValue = 0;    // Default value for unknown types
      }

      keyValue.dataTypeName = GetDataTypeName(valueType);

      // Add the value information to the vector
      values.push_back(keyValue);
    }
  } catch (const std::exception& ex) {
    // Handle the exception (e.g., log or report)
    std::wcerr << L"Exception while enumerating CurrentUser values: "
               << ex.what() << std::endl;
    // Return an empty vector on exception
    return {};
  }

  return values;
}
