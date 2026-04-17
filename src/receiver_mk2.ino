#include <SPI.h>
#include <RF24.h>
#include <Crypto.h>
#include <AES.h>
#include <string.h>

#define CE_PIN 4
#define CSN_PIN 5

    byte key[16] = { /* nothing to see here */ };


    RF24 radio(CE_PIN, CSN_PIN);
    const byte addresses[][6] = {"sen", "rec"};

    AES128 aes128;

    bool receiving = false;
    unsigned long lastPacketTime = 0;
    const int TIMEOUT_MS = 2000;
    byte ackPacket[2];
    void setup() {
        Serial.begin(115200);
        delay(2000);

        aes128.setKey(key, sizeof(key));

        Serial.println("Starting nRF24 Receiver...");
        Serial.print("&&&");
        SPI.begin();
        SPI.setFrequency(4000000); //4mhz spi speed
        if (!radio.begin()) {
            Serial.println("nRF24 init failed!");
            while (1);
        }

        radio.enableDynamicPayloads();  // for changing resolution etc.
        radio.enableAckPayload();

        radio.setPALevel(RF24_PA_MAX);     // max power
        radio.setDataRate(RF24_2MBPS);     // max speed
        radio.setRetries(15, 15);          // max retry count & delay
        radio.setAutoAck(true);            // keep ACKs
        radio.setAddressWidth(3);          // address größe auf 3 bytes, wird bei jedem packet mit gesendet
        radio.openWritingPipe(addresses[1]);
        radio.openReadingPipe(1, addresses[0]);

        radio.writeAckPayload(1, &ackPacket, sizeof(ackPacket)); // only works if called once in setup somehow
        radio.startListening();
    }
    int chunkSize = 32;
    void loop() {

        if(Serial.available()) { // get new res and quality over serial
            byte res = Serial.read();   // resolution (0-11)
            byte qual = Serial.read();  // quality (0-62)

            ackPacket[0] = res;
            ackPacket[1] = qual;
            radio.writeAckPayload(1, &ackPacket, sizeof(ackPacket));
        }

        if (radio.available()) {
            receiving = true;
            uint8_t packet[chunkSize];
            radio.read(&packet, sizeof(packet));
            byte decrypted[chunkSize];
            aes128.decryptBlock(decrypted,packet);
            aes128.decryptBlock(decrypted + 16,packet + 16);
            Serial.write(decrypted, sizeof(decrypted));
            lastPacketTime = millis();
            delayMicroseconds(1);
        }

        if (receiving && millis() - lastPacketTime > TIMEOUT_MS) {
            Serial.print("===");
            receiving = false;
            delayMicroseconds(2);
        }
    }
