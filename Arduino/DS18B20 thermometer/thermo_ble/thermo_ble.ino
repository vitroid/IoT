/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux

    以下のプログラムをDS18B20用に書換えた。
    https://github.com/AmbientDataInc/EnvSensorBleGw/tree/master/src/envSensor_esp32/BLE_BME280_bcast
*/

// #ifdef ARDUINO_M5Stack_Core_ESP32
// #include <M5Stack.h>
// #endif
#include "M5Atom.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "esp_sleep.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// #include "bme280_i2c.h"

#define T_PERIOD     1   // Transmission period
#define S_PERIOD     0  // Silent period
RTC_DATA_ATTR static uint8_t seq; // remember number of boots in RTC Memory

#define ONE_WIRE_BUS 25    // Pin No, M5Atom

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// #ifdef ARDUINO_M5Stack_Core_ESP32
// #define SDA 21
// #define SCL 22
// #else
// #define SDA 12
// #define SCL 14
// #endif

// BME280 bme280(BME280_I2C_ADDR_PRIM);
// struct bme280_data data;

void setAdvData(BLEAdvertising *pAdvertising, float tmp) {
    // bme280.get_sensor_data(&data);
    // Serial.printf("temp: %.1f, humid: %.1f, press: %.1f\r\n", data.temperature, data.humidity, data.pressure / 100);
    uint16_t temp = (uint16_t)(tmp * 100);
    // uint16_t humid = (uint16_t)(data.humidity * 100);
    // uint16_t press = (uint16_t)(data.pressure / 10);

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

    oAdvertisementData.setFlags(0x06); // BR_EDR_NOT_SUPPORTED | LE General Discoverable Mode

    std::string strServiceData = "";
    strServiceData += (char)0x06;   // 長さ
    strServiceData += (char)0xff;   // AD Type 0xFF: Manufacturer specific data
    strServiceData += (char)0xff;   // Test manufacture ID low byte
    strServiceData += (char)0xff;   // Test manufacture ID high byte
    strServiceData += (char)seq;                   // シーケンス番号
    strServiceData += (char)(temp & 0xff);         // 温度の下位バイト
    strServiceData += (char)((temp >> 8) & 0xff);  // 温度の上位バイト

    oAdvertisementData.addData(strServiceData);
    pAdvertising->setAdvertisementData(oAdvertisementData);
}


BLEServer *pServer;
BLEAdvertising *pAdvertising;

void setup() {
    M5.begin(true, false,
             true);  // Init Atom-Matrix(Initialize serial port, LED).  初始化

    Serial.begin(115200);
    Serial.printf("start ESP32 %d\n",seq);

    // initialize the thermometer ds18b20
    sensors.begin();

    // bme280.begin(); // BME280の初期化

    BLEDevice::init("Thermo-01");                  // デバイスを初期化
    pServer = BLEDevice::createServer();
    pAdvertising = pServer->getAdvertising(); // アドバタイズオブジェクトを取得
}

void loop() {
    sensors.requestTemperatures();
    Serial.print("sensor(0)\t");
    float tmp = sensors.getTempCByIndex(0);
    Serial.println(tmp);

    setAdvData(pAdvertising, tmp);                          // アドバタイジングデーターをセット

    pAdvertising->start();                             // アドバタイズ起動
    Serial.println("Advertizing started...");
    delay(T_PERIOD * 1000);                            // T_PERIOD秒アドバタイズする
    pAdvertising->stop();                              // アドバタイズ停止

    seq++;                                             // シーケンス番号を更新

    // Serial.printf("enter deep sleep\n");
    delay(S_PERIOD * 1000);
    // esp_deep_sleep(1000000LL * S_PERIOD);              // S_PERIOD秒Deep Sleepする
    // Serial.printf("in deep sleep\n");
}
