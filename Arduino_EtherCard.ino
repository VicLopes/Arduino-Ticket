#include <EtherCard.h>

// Defines ENC28J60 MAC address. Must be unique on LAN.
static uint8_t mymac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};

/* In case of static IP
 *  
const static uint8_t ip[] = {192,168,0,100}; IP
const static uint8_t gw[] = {192,168,0,1}; Gateway
const static uint8_t dns[] = {192,168,0,130}; DNS

*/

const char website[] PROGMEM = "site-do-qr.heroku.com";
static byte session;

byte Ethernet::buffer[700]; //Buffer em 700 octetos
Stash stash;

char qrcode[36];

static void httpost () {
  Serial.println("Postando no servidor a string %s");
  byte sd = stash.create();
  stash.println(qrcode);
  stash.save();
  int stash_size = stash.size();

  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST http://$F/update HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  website, website, stash_size, sd);

  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}

char obterqr() {
  Serial.readBytes(qrcode, 36);
  Serial.print("Leitura completa do código: ");
  Serial.println(qrcode);
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);/
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  ether.printIp("IP:   ", ether.myip); // output IP address to Serial
  ether.printIp("GW:   ", ether.gwip); // output gateway address to Serial
  ether.printIp("DHCP server: ", ether.dhcpip); // output IP address of the DHCP server

  /* For testing with static IP adresses, not currently implemented according to EtherCard library
  if(!ether.staticSetup(ip, gw, dns);
  {
      // handle failure to configure static IP address (current implementation always returns true!)
  }
  */
}

void loop() {
  obterqr();
  httpost();
}

