#include "RegistryInterface.h"

// Function to convert DWORD data type to name
std::wstring
RegistryInterface::GetDataTypeName(DWORD dataType)
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
          RegistryValue registryValue;
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
