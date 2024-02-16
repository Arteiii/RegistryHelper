#pragma once

#include "pch.h"

struct RecoverySettings
{
  std::wstring recoveryOption;
  std::wstring dumpFileLocation;
  // Add other settings as needed...
};

// Define a struct to hold registry value information
struct RegistryValue
{
  std::wstring name;
  std::wstring stringValue;
  DWORD dwordValue;
  std::wstring dataTypeName;

  // Constructor to initialize members
  RegistryValue()
    : name()
    , stringValue()
    , dwordValue(0)
    , dataTypeName()
  {
  }
};

struct ServiceInfo
{
  std::wstring serviceName;
  std::vector<RegistryValue> values;
};

class RegistryInterface
{
public:
  RegistryInterface();

  // Methods related to service information
  std::vector<ServiceInfo> EnumerateServices();

  // Methods related to recovery settings
  RecoverySettings GetAllSettings();

  std::wstring GetDataTypeName(DWORD dataType) noexcept;

private:
  RegistryHelper registryHelper; // Member variable

  // Constants for registry keys
  static const wchar_t* CrashControlKey;

  // Private helper functions
  inline std::wstring GetAutoRebootSetting();
  inline std::wstring GetMiniDumpDirSetting();
};
