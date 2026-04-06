import serial
import time

port = "/dev/ttyUSB1"
ser = serial.Serial(port, 115200, timeout=0)
# had to set timeout to non blocking so that the images are still written in case of connection loss

while True:
    capturing = True
    filename = "received" + str(int(time.time() * 1000)) + ".jpg" # every image a different name so I dont overwrite the old ones

    with open(filename, "wb") as f: # wb is write and binary for bytes writing
        print("Listening for image...")
        while capturing:
            data = ser.read(1024) # read the serial data in chunks and not every byte it self
            if data:
                f.write(data) # write the image also in chunks so that I can see the image building itself when it updates
                if b"===" in data: # here is my custom "end of image" indicator
                    capturing = False
                    print("Image saved as:", filename) # notify the command line so I know when an image is fully received
