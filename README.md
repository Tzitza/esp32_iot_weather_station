# ESP32 IoT Weather Station

A smart weather monitoring system built with ESP32 that reads environmental data and displays it both locally and in the cloud.

## Features

- **Real-time Monitoring**: Continuously reads temperature and humidity from a DHT11 sensor
- **Local Display**: Shows live readings on a 16x2 I2C LCD display with WiFi connection status indicator
- **Cloud Integration**: Sends data to Thinger.io IoT platform for remote monitoring
- **Smart Averaging**: Calculates and transmits 5-minute average temperature readings
- **Remote Control**: Control GPIO pin 4 remotely via Thinger.io
- **WiFi Status**: Visual indicator on LCD showing connection status using custom characters
