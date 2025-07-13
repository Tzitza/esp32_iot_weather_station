# ESP32 IoT Weather Station

A smart weather monitoring system built with ESP32 that reads environmental data and displays it both locally and in the cloud.

## Features

- **Real-time Monitoring**: Continuously reads temperature and humidity from a DHT11 sensor
- **Local Display**: Shows live readings on a 16x2 I2C LCD display with WiFi connection status indicator
- **Cloud Integration**: Sends data to Thinger.io IoT platform for remote monitoring
- **Smart Averaging**: Calculates and transmits 5-minute average temperature readings
- **Remote Control**: Control GPIO pin 4 remotely via Thinger.io
- **WiFi Status**: Visual indicator on LCD showing connection status using custom characters

## Hardware Requirements

- ESP32 microcontroller
- DHT11 temperature and humidity sensor
- 16x2 I2C LCD display (address 0x27)
- Jumper wires and breadboard

## Connections

- DHT11 sensor → GPIO 2
- LCD display → I2C pins (SDA/SCL)
- Optional: LED or relay on GPIO 4 for remote control

## Software Dependencies

- ThingerESP32 library
- DHT sensor library
- LiquidCrystal_I2C library
- Wire library (for I2C communication)

## Setup

1. Install required libraries in Arduino IDE:
   - ThingerESP32
   - DHT sensor library
   - LiquidCrystal_I2C
   - Wire (usually included)

2. Edit `arduino_secrets.h` with your actual credentials:
   - Replace placeholder values with your Thinger.io account details
   - Add your WiFi network name and password

3. Set up a bucket in Thinger.io with ID "bucketid"

4. Connect your hardware according to the connections section

5. Upload the code to your ESP32

## How it Works

The system operates in a continuous loop:
- Reads temperature and humidity every 2 seconds
- Displays current readings on the LCD
- Accumulates temperature readings for 5-minute averages
- Sends both real-time and averaged data to Thinger.io
- Updates WiFi connection status indicator on the display

## Data Output

- **Local**: Temperature and humidity displayed on LCD
- **Cloud**: Real-time data sent to Thinger.io bucket
- **Averaged**: 5-minute temperature averages for trend analysis

## Customization

- Change sensor pin by modifying `DHTPIN`
- Adjust LCD I2C address if different from 0x27
- Modify averaging interval by changing the 300000ms value
- Update bucket ID in the `write_bucket` function

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
