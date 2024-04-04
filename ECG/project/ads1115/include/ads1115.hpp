#ifndef ADS1115_HPP
#define ADS1115_HPP

/*
 * Edited ADS1115 class to read data at a given sampling rate from a file mocking the ADS1115.
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 * Copyright (c) 2013-2022  Bernd Porr <mail@berndporr.me.uk>
 * Copyright (c) 2024 Jamie Milsom <*******@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 */

// Include any necessary headers here
#include <assert.h>
// #include <pigpio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/lockfree/spsc_queue.hpp>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/file.h>  // flock
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>


struct ADS1115settings {
  enum SamplingRates {
    FS8HZ = 0,
    FS16HZ = 1,
    FS32HZ = 2,
    FS64HZ = 3,
    FS128HZ = 4,
    FS250HZ = 5,
    FS475HZ = 6,
    FS860HZ = 7
  };

  inline unsigned getSamplingPeriodMs() const {
    const float SamplingRateEnum2Value[8] = {125.0, 62.5, 31.25, 15.625,
                                                7.8125, 4.0, 2.105, 1.1628};
    return SamplingRateEnum2Value[samplingRate];
  }

  SamplingRates samplingRate = FS860HZ;

};


class ADS1115 {
 public:
  void start();


  ~ADS1115() { stop(); }


  boost::lockfree::spsc_queue<float, boost::lockfree::capacity<1024>>
      ads1115queue;

  /**
   * Called when a new sample is available.
   * This needs to be implemented in a derived
   * class by the client. Defined as abstract.
   * \\param sample Voltage from the selected channel.
   **/
  virtual void hasSample(float v) = 0;

  /**
   * Stops the data acquistion
   **/
  ADS1115settings getADS1115settings() const { return ads1115settings; }

  void stop();

 private:
  ADS1115settings ads1115settings;
  void dataReady();

  // Private member variables and functions
};


class mockADS1115 : public ADS1115 {
public:
    void hasSample(float v) override {
        // Implement what to do with the sample here
        std::cout << "ECG value before queue: " << v << std::endl;
        while (!ads1115queue.push(v)) {
            std::this_thread::yield();  // Yield if queue is full
        }
    }
};


#endif  // ADS1115_HPP