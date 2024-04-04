#include "ecg.hpp"
#include "ads1115.hpp"

/**
 * @brief Starts the ECG sensor.
 */
ECG::ECG() {
  // Add your code here
}

void ECG::start(std::unique_ptr<ADS1115>& ads_ptr) {
  std::cout << "ECG sensor started" << std::endl;
  float value;
  while (true) {
    if (!ads_ptr->ads1115queue.pop(value)) {
      std::this_thread::yield();
    } else {
      std::cout << "ECG value: " << value << std::endl;
      // Process value
    }
  }
}

void ECG::stop() {
  // Add your code here
}

void ECG::GetECGData() {
  // Add your code here
}

/**
 * @brief Stops the ECG sensor.
 */
ECG::~ECG() {
  // Add your code here
}


int main() {
    std::unique_ptr<mockADS1115> ads1115 = std::make_unique<mockADS1115>();
    ads1115->start();
    ads1115->stop();
    return 0;
}