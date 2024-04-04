
#include "ads1115.hpp"


void ADS1115::dataReady() {
    std::ifstream datFile("/home/jamie-linux/Documents/realtime_circular_buffer/ECG/project/data/ECG_1000Hz_7.dat");
    unsigned samplingInterval = getADS1115settings().getSamplingPeriodMs();
    if (!datFile.is_open()) {
        std::cerr << "Error opening file: " << "../../data/ECG_1000Hz_7.dat" << std::endl;
    }
    float value;
    while (datFile >> value) {
        hasSample(value);
        std::this_thread::sleep_for(std::chrono::milliseconds(samplingInterval));
    }
    datFile.close();
}

void ADS1115::hasSample(float v) {
  // Implement the function here
  std::cout << "ECG value before queue: " << v << std::endl;
  while (!ads1115queue.push(v)) {
    std::this_thread::yield();  // Yield if queue is full}
  }
}


void ADS1115::start() {
    // Start the data acquisition
    dataReady();
}

void ADS1115::stop() {
    // Stop the data acquisition
    std::cout << "Stopping data acquisition" << std::endl;
}
