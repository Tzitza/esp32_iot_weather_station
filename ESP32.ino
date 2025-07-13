/*
  Project: ESP32 IoT Weather Station with Thinger.io and LCD Display

  Description:
  ------------
  This project implements a simple IoT weather station using an ESP32 microcontroller.
  It reads temperature and humidity data from a DHT11 sensor and displays the readings
  on a 16x2 I2C LCD display. The device also connects to the Thinger.io IoT platform,
  sending both real-time and 5-minute average temperature/humidity data to the cloud.

  Key Features:
  -------------
  - Reads temperature and humidity from a DHT11 sensor.
  - Displays live sensor readings on an I2C LCD (address 0x27).
  - Shows WiFi connection status on the LCD using custom characters.
  - Calculates and sends the 5-minute average temperature to Thinger.io.
  - Sends live temperature and humidity values to Thinger.io.
  - Controls a GPIO pin (pin 4) remotely via Thinger.io.
  - Modular structure for easy adaptation and extension.

  Libraries Used:
  ---------------
  - ThingerESP32: For Thinger.io integration.
  - DHT: For DHT11 sensor communication.
  - LiquidCrystal_I2C: For controlling the LCD display.
  - Wire: For I2C communication.

  Customization:
  --------------
  - WiFi credentials and Thinger.io credentials are stored in "arduino_secrets.h".
  - LCD custom characters indicate WiFi connection state.
  - Bucket ID for Thinger.io data writes should be set accordingly.

  Usage:
  ------
  1. Connect the DHT11 sensor to GPIO 2.
  2. Connect a 16x2 LCD with I2C interface (address 0x27).
  3. Add your WiFi and Thinger.io credentials in "arduino_secrets.h".
  4. Upload the code to your ESP32.
  5. View live sensor data on the LCD and remotely via Thinger.io.
  
*/
#define THINGER_SERIAL_DEBUG

#include <ThingerESP32.h>
#include "arduino_secrets.h"
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Αρχικοποίηση της LCD με διεύθυνση I2C 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Μεταβλητές για τον υπολογισμό του μέσου όρου θερμοκρασίας ανά 5 λεπτά
float temperatureSum = 0;  // Άθροισμα των μετρήσεων θερμοκρασίας
int readingCount = 0;      // Αριθμός μετρήσεων
unsigned long startTime = 0; // Χρόνος έναρξης του 5λεπτου
float avgTemperature = 0;  // Μέση θερμοκρασία ανά 5 λεπτά

bool isConnected = false;

// Προσαρμοσμένοι χαρακτήρες LCD για WiFi κατάσταση
byte filledDot[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte openCircle[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01010,
  0b01010,
  0b00100,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(115200);
  dht.begin();

  lcd.init();
  lcd.backlight();

  // Δημιουργία προσαρμοσμένων χαρακτήρων
  lcd.createChar(0, filledDot);
  lcd.createChar(1, openCircle);

  pinMode(4, OUTPUT);
  thing["GPIO_16"] << digitalPin(4);

   
  // Σύνδεση στο WiFi
  thing.add_wifi(SSID, SSID_PASSWORD);

  // Ορισμός πηγής δεδομένων για το Thinger.io
  thing["TempHum"] >> [](pson &out) {
    out["temperature"] = dht.readTemperature(); // Θερμοκρασία
    out["humidity"] = dht.readHumidity();       // Υγρασία
  };

  // Νέα πηγή για τον μέσο όρο θερμοκρασίας
  thing["AvgTemp"] >> [](pson &out) {
    out["val"] = avgTemperature; // Αποστολή μέσης θερμοκρασίας στην τιμή του bucket
  };

  startTime = millis(); // Αρχικοποίηση του χρόνου έναρξης
}

void calculateAverageTemperature() {
  // Ανάγνωση της τρέχουσας θερμοκρασίας
  float currentTemperature = dht.readTemperature();
  if (!isnan(currentTemperature)) {
    temperatureSum += currentTemperature; // Προσθήκη θερμοκρασίας στο άθροισμα
    readingCount++;                        // Αύξηση του αριθμού μετρήσεων
  }

  // Έλεγχος αν έχουν περάσει 5 λεπτά (300.000 ms)
  if (millis() - startTime >= 300000) {
    if (readingCount > 0) {
      avgTemperature = temperatureSum / readingCount; // Υπολογισμός μέσου όρου
    } else {
      avgTemperature = NAN; // Διαχείριση αν δεν υπάρχουν έγκυρες μετρήσεις
    }

    temperatureSum = 0;
    readingCount = 0;
    startTime = millis(); // Επαναφορά χρόνου έναρξης
  }
}

void loop() {
  calculateAverageTemperature(); // Υπολογισμός μέσης θερμοκρασίας
  updateConnectionStatus(); // Ενημέρωση κατάστασης σύνδεσης Wi-Fi στο LCD

  thing.handle(); // Διαχείριση Thinger.io
  thing.write_bucket("bucketid", "TempHum"); // Αποστολή δεδομένων θερμοκρασίας και υγρασίας

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C ");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %  ");

  delay(2000); // Μικρή καθυστέρηση για σταθερότητα
}

void updateConnectionStatus() {
  lcd.setCursor(15, 0); // Κορυφή δεξιά γωνία
  if (WiFi.status() == WL_CONNECTED) {
    lcd.write(0); // Γεμάτο κουκί για συνδεδεμένο
  } else {
    lcd.write(1); // Κενό κύκλος για αποσυνδεδεμένο
  }
}