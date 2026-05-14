# Wireless Esp32-Camera using nRF24l01+

### What is it?

I'm building a wireless esp32 camera that uses the 2.4 GHZ RF chip nRF24l01+ to transmit the images over a greater distance than just WIFI. Using the Ebyte E01 2G4M27D Module achieving distances up to 3000 meters, I can receive images and change the resolution and quality of the captures from the receiver side.

### But Why?

My Wifi signal doesn't cover my entire property so I figured I will build a security camera that has a higher distance and can be placed on the edge where no normal camera would get a connection.
Also, out on my field, I can monitor all my stuff that's stored there from home and watch the animals walk around at night from a few hundred meters away.

### How it works


#### The Transmitter
The image is captured and divided into chunks of 32 bytes.

Then those chunks are split into 16 byte chunks for separately encrypting them using AES-128 (symmetric encryption).

Those chunks are transmitted on 2.4 ghz on a high channel like 74 preventing conflicts with wifi.

The nRF24 uses a proprietary protocol with GFSK (Gaussian Frequency Shift Keying).

![](https://github.com/F45c/wireless-esp32-cam/blob/4ec442fce8e9c3328b5b851075165537184033eb/images/transmission_process.svg)


#### The Receiver
The receiver separates the 32 byte chunks into 16 byte chunks for decryption following the transmission over Serial to the Computer.

The Computer runs a python script saving the data to an image file.



### 3D Model
![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_model_open.png)
![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_model_closed.png)

### Prototyping

Using some cheaper modules and some perfboards, I made a prototype which I used to transmit and receive this image.

It's still low quality because of the bad prototype hardware. The highest distance I achieved with the prototype was ~100m line-of-sight and in another test, the signal was able to pass through two walls (steel-concrete 30cm) without much data loss.

These are the speeds the prototype achieved.

|Resolution & Quality|	Time|	FPS|
|--------------|----|----|
|160x120 + low jpeg quality| <1s|	~2.5|
320x240 + low jpeg quality|	~1.5s	| ~0.6|
480x320 + low jpeg quality|	~3s|	~0.3|
800x600 + little higher quality| ~15s| ~0.07|
1600x1200 + low jpeg quality|	~50s|	~0.02|

The final build with the PCB should be able to send higher quality and resolution images with less error rate and faster speeds. How fast the 2G4M27D is impossible to know but I'm assuming an increase of at least 30% in transmission speed. 

Since the datasheet claims a range of 5000m LOS, I'm assuming that the final build will achieve a functioning range of about 2500m LOS when placing the antennas correctly at a high place. This would be definitely enough for my use case.

![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/received_image.png)

### Schematic

I use PinSockets for both the Esp32 and the 2G4M27D and PinHeaders for the Esp32 UART interface. I added two pads for the 3.7v lithium ion battery and it's connected to the VCC pin of the Esp32 which is internally converted into 3.3v for the output pin.

This is the Pinout:
|Esp32|2G4M27D|
|-----|------|
|3V3|3V3|
|GND|GND|
|GPIO12|CE|
|GPIO33|CNS|
|GPIO14|SCK|
|GPIO25|MOSI|
|GPIO27|MISO|
|GPIO26|IRQ|

![](https://github.com/F45c/wireless-esp32-cam/blob/86789df27dbef6ad9e9af5e736c8ab6f7e268cf3/images/schematic.png)


### PCB Design
<p float="left">
  <img src="https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_pcb_final_touch.png" alt="" style="width:40%; height:100%;">
  <img src="https://github.com/F45c/wireless-esp32-cam/blob/393d8cde2f65eac39da4fa23c8ae1cbebf0155d2/images/3d_pcb_with_wrover.png" alt="" style="width:40%; height:100%;">
</p>

![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/pcb_final_touch.png)

### BOM

|Name|Purpose|Quantity|Total Cost (USD)|Link|Distributor|
|----|------|---|------|------------|-----------|
|3.7v LiPo battery|Battery for transmitter|1|21.18|https://www.conrad.de/de/p/akyga-lp675568-akkupack-x-spezial-akku-lipo-3-7-v-3500-mah-3385862.html|Conrad|
|PCB|PCB for both receiver and transmitter|1|5.5||JLCPCB|
|Esp32 38 pin|Receiver Esp32|1|5.73|https://de.aliexpress.com/item/1005006613312645.html|Aliexpress|
|Esp32 Wrover Cam OV5640|Capturing images and computing transmission|1|9|https://de.aliexpress.com/item/1005006705436186.html|Aliexpress|
|2 pieces Ebyte E01-2G4M27D|One for transmitter, one for receiver|1|11.9|https://de.aliexpress.com/item/1005003212341678.html|Aliexpress|



