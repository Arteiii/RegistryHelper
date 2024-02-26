#pragma once

struct RecoverySettings
{
  std::wstring recoveryOption;
  std::wstring dumpFileLocation;
  // Add other settings as needed...
};

// RegistryKeyValue struct to store information about each registry value
// RegistryKeyValue struct to store information about each registry value
struct RegistryKeyValue
{
  std::wstring name;
  std::wstring stringValue;
  DWORD dwordValue;
  std::wstring dataTypeName;
  std::wstring keyName;
  std::vector<RegistryKeyValue> values;

  // Constructor to initialize members
  RegistryKeyValue()
    : name()
    , stringValue()
    , dwordValue(0)
    , dataTypeName()
    , keyName()
    , values()
  {
  }

  // Constructor to initialize members with values
  RegistryKeyValue(const std::wstring& _name,
                   const std::wstring& _stringValue,
                   DWORD _dwordValue,
                   const std::wstring& _dataTypeName)
    : name(_name)
    , stringValue(_stringValue)
    , dwordValue(_dwordValue)
    , dataTypeName(_dataTypeName)
    , keyName()
    , values()
  {
  }
};

struct ServiceInfo
{
  std::wstring serviceName;
  std::vector<RegistryKeyValue> values;
};

class RegistryInterface
{
public:
  // RegistryInterface(); not implemented

  // Methods related to service information
  std::vector<ServiceInfo> EnumerateServices();

  // Methods related to recovery settings
  RecoverySettings GetAllSettings();

  std::wstring GetDataTypeName(DWORD dataType) noexcept;

  std::vector<RegistryKeyValue> EnumerateCurrentUserValues();

private:
  RegistryHelper registryHelper; // Member variable

  // Constants for registry keys
  static const wchar_t* CrashControlKey;

  // Private helper functions
  inline std::wstring GetAutoRebootSetting();

  inline std::wstring GetMiniDumpDirSetting();
};
