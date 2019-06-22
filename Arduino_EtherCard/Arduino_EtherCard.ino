#include <EtherCard.h>
#include <ArduinoJson.h>

// Defines ENC28J60 MAC address. Must be unique on LAN.
static uint8_t mymac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};
/*
static byte ip[] = {192,168,0,100}; //IP
static byte gw[] = {192,168,0,1}; //Gateway
static byte dns[] = {192,168,0,130}; //DNS
*/
const char website[] PROGMEM = "ticket-project.herokuapp.com"; //Uses Port 80
static byte session;

byte Ethernet::buffer[700]; //Buffer em 700 octetos
Stash stash;

char qrcode[36];

bool httpost () {
  StaticJsonDocument<70> doc;
  doc["qr_code"] = qrcode;
  serializeJson(doc, Serial);
  
  Serial.println("Postando no servidor a string");
  byte sd = stash.create();
  serializeJson(doc, stash);
  stash.save();
  int stash_size = stash.size();

  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST https://$F/api/tickets HTTP/1.1" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  website, website, stash_size, sd);

  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
  return true;
}

char obterqr() {
  Serial.readBytes(qrcode, 36);
  Serial.print("Leitura completa do código: ");
  Serial.println(qrcode);
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  ether.printIp("IP:   ", ether.myip); // output IP address to Serial
  ether.printIp("GW:   ", ether.gwip); // output gateway address to Serial
  ether.printIp("DHCP server: ", ether.dhcpip); // output IP address of the DHCP server

  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));

  ether.printIp("SRV: ", ether.hisip);

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
  ether.packetLoop(ether.packetReceive());

  const char* reply = ether.tcpReply(session);
  if (reply != 0) {
    Serial.println("Got a response!");
    Serial.println(reply);
  }
  if (reply == 0)
    Serial.println("NULL reply payload.");
}

