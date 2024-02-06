#include "CppUnitTest.h"
#include "RegistryInterface.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RegistryInterfaceTests {
TEST_CLASS(RegistryInterfaceTests){
  public :

    TEST_METHOD(GetDataTypeNameTest){ RegistryInterface registryInterface;
Assert::AreEqual(L"REG_SZ", registryInterface.GetDataTypeName(REG_SZ).c_str());
Assert::AreEqual(L"REG_EXPAND_SZ",
                 registryInterface.GetDataTypeName(REG_EXPAND_SZ).c_str());
Assert::AreEqual(L"REG_BINARY",
                 registryInterface.GetDataTypeName(REG_BINARY).c_str());
Assert::AreEqual(L"REG_DWORD",
                 registryInterface.GetDataTypeName(REG_DWORD).c_str());
Assert::AreEqual(L"Unknown", registryInterface.GetDataTypeName(123).c_str());
}

TEST_METHOD(EnumerateServicesTest)
{
  // Assuming there are services present in the system registry
  RegistryInterface registryInterface;
  std::vector<ServiceInfo> services = registryInterface.EnumerateServices();

  // Ensure at least one service is found
  Assert::IsTrue(!services.empty());
}

TEST_METHOD(EnumerateServicesWithNoValuesTest)
{

  RegistryInterface registryInterface;
  std::vector<ServiceInfo> services = registryInterface.EnumerateServices();
  Assert::IsTrue(!services.empty());
  Assert::IsTrue(services[0].values.empty());
  Assert::IsTrue(services[1].values.empty());
}
}
;
}
