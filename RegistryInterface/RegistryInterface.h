#pragma once

#include "RegistryHelper.h"
#include <unordered_map>

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
  std::vector<ServiceInfo> EnumerateServices();
  std::wstring GetDataTypeName(DWORD dataType);

private:
};
