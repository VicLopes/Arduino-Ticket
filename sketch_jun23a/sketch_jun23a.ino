#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "ticket-project.herokuapp.com";

static byte session;
byte Ethernet::buffer[700];
Stash stash;

char qrcode[] = "90b1ba8e-2f8b-48c1-aa5a-934200316df9";
char oldcode[36];
void setup () {
  Serial.begin(57600);
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
     Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
     Serial.println(F("DHCP failed"));
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
  
  if (!ether.dnsLookup(website))
     Serial.println(F("DNS failed"));
  
  ether.printIp("SRV: ", ether.hisip);
  
  getReq();

}

void loop () {
  
  ether.packetLoop(ether.packetReceive());
  
  const char* reply = ether.tcpReply(session);
  if (reply != 0) {
    Serial.println(reply);
  }

  
}


static void getReq () {
  Serial.print("Sending QR code:");
  Serial.println(qrcode);
  byte sd = stash.create();
  
  stash.print("qr_code=");
  stash.println(qrcode);
  stash.save();
  int stash_size = stash.size();
  
  
  Stash::prepare(PSTR("POST /api/tickets HTTP/1.1" 
                      "\r\n"
                      "Host: $F" "\r\n"
                      "User-Agent: Arduino/1.0" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "Content-Type: application/x-www-form-urlencoded" "\r\n"
                      "\r\n"
                      "$H"),
                      website, stash_size, sd);
  
  
  
  
  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}

char obterqr() {
  Serial.readBytes(qrcode, 36);
  Serial.print("Leitura completa do codigo: ");
  Serial.println(qrcode);
  delay(1000);
}
