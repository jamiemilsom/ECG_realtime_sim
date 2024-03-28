#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include "Iir.h"

// simulated sampling params
const int SAMPLING_RATE = 1000; // Hz
std::vector<float> ecg_data;

// infinite impulse response library filter paramaeters
const int filter_order = 4;
const float cutoff_frequency = 50.0f; // Hz (EMG / Power line removal)
const float highpass_cutoff_frequency = 0.1f; // Hz (baseline wander removal)

// circlular buffer
const int BUFFER_SIZE = 2000;

// biological data
std::vector<int> detected_peaks;  // Store indices of detected R peaks

bool read_ecg_data(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return false;
    }
    float value;
    while (file >> value) {
        ecg_data.push_back(value);
    }
    file.close();
    return true;
}

float HighPassThenBandpass(Iir::Butterworth::BandPass<filter_order>& bandpass_filter,
                           Iir::Butterworth::HighPass<filter_order>& highpass_filter,
                           float sample) {

    float filtered_sample_hp = highpass_filter.filter(sample);
    return bandpass_filter.filter(filtered_sample_hp);
}

float calculate_heart_rate(const std::vector<int>& peak_indices) {
    if (peak_indices.empty()) {
        return 0.0f; // if no peaks detected, return 0 heart rate
    }


    float average_time_between_peaks = 0.0f;
    for (int i = 1; i < peak_indices.size(); i++) {
        int time_between_peaks = peak_indices[i] - peak_indices[i - 1];
        average_time_between_peaks += time_between_peaks;
    }
    average_time_between_peaks /= (peak_indices.size() - 1);

    float heart_rate = 60.0f / (average_time_between_peaks / SAMPLING_RATE);
    return heart_rate;
}

void display_buffer(const std::vector<float>& buffer, int head, int tail) {
  for (int i = 0; i < buffer.size(); i++) {
    int index = (head + i) % BUFFER_SIZE;
    std::cout << buffer[index] << " ";
  }
  std::cout << std::endl;
}




int main() {

    if (!read_ecg_data("../../data/ECG_1000Hz_7.dat")) {
        return 1;
    }

    // Butterworth bandpass filter for removing EMG / power line noise
    Iir::Butterworth::BandPass<filter_order> bandpass_filter;
    bandpass_filter.setup(SAMPLING_RATE, 0.5f, cutoff_frequency);

    // Butterworth high-pass filter for baseline wander removal
    Iir::Butterworth::HighPass<filter_order> highpass_filter;
    highpass_filter.setup(SAMPLING_RATE, highpass_cutoff_frequency);


    std::vector<float> buffer(BUFFER_SIZE, 0.0f);
    int head = 0;  // most recent sample in the buffer (index)
    int tail = 0;  // oldest sample in the buffer (index)
    
    int n_readings_disp = 1e9;
    int ecg_size = ecg_data.size();
    int n_readings = std::min(n_readings_disp,ecg_size);

    float threshold = 0.5f; 

    for (int i = 0; i < n_readings; i++) {

        float raw_sample = ecg_data[i];

        float filtered_sample = HighPassThenBandpass(bandpass_filter, highpass_filter, raw_sample);

        if (filtered_sample > threshold && 
            (detected_peaks.empty() || (i - detected_peaks.back()) > 200)) { // Minimum distance check
            detected_peaks.push_back(i);
        }

        buffer[head] = filtered_sample;
        head = (head + 1) % BUFFER_SIZE;

        // display_buffer(buffer, head, tail);
        float heart_rate = calculate_heart_rate(detected_peaks);
        std::cout << "Heart Rate (bpm): " << heart_rate << std::endl;

    }
    return 0;
}
