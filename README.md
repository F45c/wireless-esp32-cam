[Wireless Esp32-Cam-bom.csv](https://github.com/user-attachments/files/26871883/Wireless.Esp32-Cam-bom.csv)# Wireless Esp32-Camera using nRF24l01+

### What is it?

I'm building a wireless esp32 camera that uses the 2.4 GHZ RF chip nRF24l01+ to transmit the images over a greater distance than just WIFI. Using the Ebyte E01 2G4M27D Module achieving distances up to 3000 meters, I can receive images and change the resolution and quality of the captures from the receiver side.

### But Why?

My Wifi signal doesn't cover my entire property so I figured I will build a security camera that has a higher distance and can be placed on the edge where no normal camera would get a connection.
Also, out on my field, I can monitor all my stuff that's stored there from home and watch the animals walk around at night from a few hundred meters away.

### 3D Model
![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_model_open.png)
![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_model_closed.png)

### Prototyping

Using some cheaper modules and some perfboards, I made a prototype which I used to transmit and receive this image

![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/received_image.png)

### PCB Design

![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/3d_pcb_final_touch.png)
![](https://github.com/F45c/wireless-esp32-cam/blob/4302deed90e64e07422e6db8d4a3dc765364d484/images/pcb_final_touch.png)

### BOM

|Name|Purpose|Quantity|Total Cost (USD)|Link|Distributor|
|----|------|---|------|------------|-----------|
|PCB|PCB for both receiver and transmitter|1|5.5||JLCPCB|
|Esp32 38 pin|Receiver Esp32|1|5.73|https://de.aliexpress.com/item/1005006613312645.html|Aliexpress|
|Esp32 Wrover Cam OV5640|Capturing images and computing transmission|1|9|https://de.aliexpress.com/item/1005006705436186.html|Aliexpress|
|2 pieces Ebyte E01-2G4M27D|One for transmitter, one for receiver|1|11.9|https://de.aliexpress.com/item/1005003212341678.html|Aliexpress|



