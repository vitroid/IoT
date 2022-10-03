#include <M5Atom.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 25    // Pin No

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
    M5.begin(true, false, true);  // use LED
    delay(50);

    M5.dis.drawpix(0, 0xf00000); // green
    delay(50);

    Serial.begin(115200);
    Serial.print("start M5Atom-lite\n");

   sensors.begin();
}

void loop() {
    if (M5.Btn.wasPressed()){
        Serial.print("button Pushed\n");
        delay(50);
        M5.dis.drawpix(0, 0x00f000); // RED
        delay(50);
        M5.update();
    }
    M5.update();
   sensors.requestTemperatures();
   Serial.print("sensor(0)\t");
   float tmp = sensors.getTempCByIndex(0);
   Serial.println(tmp);
   delay(1000);
}
