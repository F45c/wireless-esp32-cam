#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "esp_camera.h"


// Camera OV5640 pin setup for ESP32 WROVER
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

// nRF24L01 pins (CE, CSN)
#define CE_PIN   2
#define CSN_PIN  15

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "1Node";


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
  
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("Camera init failed");
    while (true);
  }
}

void setup() {
  Serial.begin(115200);
  setupCamera();
  SPI.begin(14, 12, 13, 15); // SCK, MISO, MOSI, SS
  // my custom SPI pins

  if (!radio.begin()) {
    Serial.println("nRF24 init failed!");
    while (1);
  }
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(address);
  radio.stopListening(); // set it in transmit mode
  
  Serial.println("Setup Done");
}


void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return true;
  }
  
  size_t imgLen = fb->len;
  const uint8_t* imgData = fb->buf;
  
  Serial.printf("%d bytes\n", imgLen); // number of bytes for current image
  
  
  // send image in 32 byte chunks
  for (size_t i = 0; i < imgLen; i += 32) {
    size_t chunkSize = (imgLen - i >= 32) ? 32 : (imgLen - i);
    radio.write(imgData + i, chunkSize);
    delayMicroseconds(5); // small delay for reliability, might remove it later
  }
  byte end[] = {'=','=','='}; // I send this as a sign for the receiver that the image ends
  radio.write(&end, sizeof(end));
  esp_camera_fb_return(fb);
  //delay(1000); // Capture interval
}
