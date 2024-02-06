#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

// Include your existing RegistryInterface and related structures
#include "RegistryInterface.h"

// Main scan loop function running in a separate thread
void
ScanLoop(RegistryInterface& registryInterface)
{
  // Initialize the previousServices vector with current services
  std::vector<ServiceInfo> previousServices =
    registryInterface.EnumerateServices();

  while (true) {
    // Enumerate services
    std::vector<ServiceInfo> currentServices =
      registryInterface.EnumerateServices();

    // Check for new services and show notifications
    for (const auto& currentService : currentServices) {
      // Check if the service is in the previous list
      auto it = std::find_if(
        previousServices.begin(),
        previousServices.end(),
        [&currentService](const ServiceInfo& previousService) {
          return previousService.serviceName == currentService.serviceName;
        });

      // If not found, it's a new service
      if (it == previousServices.end()) {
        std::wstring notificationText =
          L"New service created: " + currentService.serviceName;

        // Print the message directly instead of opening a command prompt
        std::wcout << notificationText << std::endl;
      }
    }

    // Update the previous services list for the next iteration
    previousServices = std::move(currentServices);

    // Sleep for 30 seconds before the next check
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }
}

int
main()
{
  RegistryInterface registryInterface;

  // Print server start message
  std::wcout << L"Server start" << std::endl;

  // Create a separate thread for the main scan loop
  std::thread scanThread(ScanLoop, std::ref(registryInterface));

  // Wait for user input to exit
  std::wcout << L"Press Enter to exit." << std::endl;
  std::getchar();

  // Join the scan thread before exiting
  scanThread.join();

  // Print server shutdown message
  std::wcout << L"Server shutdown" << std::endl;

  return 0;
}
