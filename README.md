Setup:
  2x Arduino UNO
  ENC28J60
  USB Host Shield

Steps:
  Connect TX port from Arduino 1 to RX port on Arduino 2, and vice-versa, forming a two-way communication between the Arduino serial ports.
  Set up ENC28J60 pins. Digital Pin 13 to ENC28J60's SCK, Pin 12 to SO, Pin 11 to SI, Pin 10 to CS, 3.3V to VCC and GND to GND.
  Afterwards, setting up the QR code reader (or any device that acts as a keyboard HID) should be as easy as plugging it into the other Arduino's USB Host Shield.
  
  A large portion of the code comes from USB Host Shield 2.0 (https://github.com/felis/USB_Host_Shield_2.0) and EtherCard (https://github.com/njh/EtherCard) possibilities.
