#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "esp_camera.h"
#include <Crypto.h>
#include <AES.h>
#include <string.h>

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      21
#define SIOD_GPIO_NUM      26
#define SIOC_GPIO_NUM      27

#define Y9_GPIO_NUM        35
#define Y8_GPIO_NUM        34
#define Y7_GPIO_NUM        39
#define Y6_GPIO_NUM        36
#define Y5_GPIO_NUM        19
#define Y4_GPIO_NUM        18
#define Y3_GPIO_NUM         5
#define Y2_GPIO_NUM         4
#define VSYNC_GPIO_NUM     25
#define HREF_GPIO_NUM      23
#define PCLK_GPIO_NUM      22

#define CE_PIN   2
#define CSN_PIN  15

RF24 radio(CE_PIN, CSN_PIN);
const byte addresses[][3] = {"sen", "rec"}; // 3 byte addresses for smaller transmission


void setupCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_UXGA; // somehow needs to be high to later use high res
    config.jpeg_quality = 2;
    config.fb_count = 2;


    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.println("Camera init failed");
        while (true);
    }
}


framesize_t framesizes[] = {
    FRAMESIZE_QQVGA,   // 160x120
    FRAMESIZE_QVGA,    // 320x240
    FRAMESIZE_CIF,     // 352x288
    FRAMESIZE_HVGA,
    FRAMESIZE_VGA,     // 640x480
    FRAMESIZE_SVGA,    // 800x600
    FRAMESIZE_XGA,     // 1024x768
    FRAMESIZE_SXGA,    // 1280x1024
    FRAMESIZE_UXGA,    // 1600x1200
    FRAMESIZE_FHD,
    FRAMESIZE_QXGA,    // 2048x1536
    FRAMESIZE_QXGA
};
const int framesizesLength = sizeof(framesizes) / sizeof(framesizes[0]);

byte key[16] = { /* nothing to see here */ };


    AES128 aes128;

    void setup() {
        Serial.begin(115200);
        setupCamera();

        sensor_t *s = esp_camera_sensor_get();
        s->set_hmirror(s, 1);
        s->set_framesize(s, FRAMESIZE_QVGA);
        s->set_quality(s, 2);

        aes128.setKey(key, sizeof(key));

        SPI.begin(14, 12, 13, 15); // SCK, MISO, MOSI, SS
        SPI.setFrequency(4000000); // 4 mhz speed was working best
        if (!radio.begin()) {
            Serial.println("nRF24 init failed!");
            while (1);
        }

        radio.enableDynamicPayloads();     // for changing res and quality
        radio.enableAckPayload();

        radio.setPALevel(RF24_PA_MAX);     // max power
        radio.setDataRate(RF24_2MBPS);     // max speed
        radio.setRetries(15, 15);          // max retry count & delay
        radio.setAutoAck(true);            // keep ACKs for changing res and quality
        radio.setAddressWidth(3);          // smaller addresses
        radio.openWritingPipe(addresses[0]);
        radio.openReadingPipe(1, addresses[1]);
        radio.stopListening();

    }

    void loop() {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            // reset settings to small images when big ones fail
            sensor_t *s = esp_camera_sensor_get();
            s->set_framesize(s, framesizes[0]);
            s->set_quality(s, 30);
            return;
        }

        size_t imgLen = fb->len;
        const uint8_t* imgData = fb->buf;

        Serial.print(imgLen);
        Serial.println(" bytes");


        for (size_t i = 0; i < imgLen; i += 32) {
            size_t chunkSize = 32;
            byte encrypted[chunkSize];
            aes128.encryptBlock(encrypted, imgData + i); // aes128 needs 16 bytes blocks
            aes128.encryptBlock(encrypted + 16, imgData + i + 16);
            radio.write(encrypted, chunkSize); // send two encrypted 16 byte blocks
            if(radio.available()) { // this is the dynamic ack payload
                // 1. byte res; 2. byte qualität
                uint8_t len = radio.getDynamicPayloadSize();
                uint8_t buf[len];
                radio.read(&buf, len);
                uint8_t newResolution = buf[0];
                uint8_t newQuality = buf[1];
                Serial.print("ACK: ");
                Serial.print(newResolution);
                Serial.print(" ");
                Serial.println(newQuality);
                sensor_t *s = esp_camera_sensor_get();
                if(newResolution >= 0 && newResolution < framesizesLength) s->set_framesize(s, framesizes[newResolution]);
                if(newQuality >= 2 && newQuality < 63) s->set_quality(s, newQuality);
                delayMicroseconds(1);
                break; // stop sending current image (because high res images takes a long time and it wouldn't switch until finished)
            }
        }
        byte end[] = { // last chunk of every image is custom end of image indicator
            '=','=','=','=', '=','=','=','=',
            '=','=','=','=', '=','=','=','=',
            '=','=','=','=', '=','=','=','=',
            '=','=','=','=', '=','=','=','='};
            byte encrypted[sizeof(end)];
            aes128.encryptBlock(encrypted, end);
            aes128.encryptBlock(encrypted + 16, end + 16);
            radio.write(encrypted, sizeof(encrypted));
            esp_camera_fb_return(fb); // free frame buffer
            return;
    }

