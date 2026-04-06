#include <SPI.h>
#include <RF24.h>

#define CE_PIN 27
#define CSN_PIN 5

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "1Node";

bool receiving = false;
long lastPacket = 0;
int timeout = 3000;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting receiver");
  if (!radio.begin()) {
    Serial.println("nRF24 init failed!");
    while (1);
  }

  //needs to be the same settings as in transmitter
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, address); // same adress as transmitter write pipe
  radio.startListening(); // receive mode
}

void loop() {
  if (radio.available()) {
    receiving = true;
    uint8_t packet[32];
    radio.read(&packet, sizeof(packet));
    Serial.write(packet, sizeof(packet)); // I write all data directly to the serial to my computer
    lastPacket = millis();
  }

  if (receiving && millis() - lastPacket > timeout) {
    Serial.print("==="); // print end of image indicator manually when receiver cant receive data anymore for 3 seconds so that the python script still saves the image
    receiving = false;
  }
}
