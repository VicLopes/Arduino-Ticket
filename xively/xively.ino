// Simple demo for feeding some random data to Pachube.
// 2011-07-08 <jc@wippler.nl>
//
// License: GPLv2

// Handle returning code and reset ethernet module if needed
// 2013-10-22 hneiraf@gmail.com

#include <EtherCard.h>
#include <ArduinoJson.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "ticket-project.herokuapp.com";

byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;
byte session;

//timing variable
int res = 0;

char qrcode[] = "1df709c5-2458-4ae7-a471-ba1c66b0e0cd";

char obterqr() {
  Serial.readBytes(qrcode, 36);
  Serial.print("Leitura completa do codigo: ");
  Serial.println(qrcode);
  delay(1000);
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[Ticket Example]");

  //Initialize Ethernet
  initialize_ethernet();
}

void loop () {

  //if correct answer is not received then re-initialize ethernet module
  if (res > 220){
    initialize_ethernet();
  }

  res = res + 1;

  ether.packetLoop(ether.packetReceive());

  //200 res = 10 seconds (50ms each res)
  if (res == 50) {
    StaticJsonDocument<61> doc;
    obterqr();
    doc["qr_code"].set(qrcode);
    Serial.print("Sending QR code:");
    Serial.println(qrcode);
    byte sd = stash.create();
    serializeJsonPretty(doc, stash);
    stash.save();
    int stash_size = stash.size();
    
    
    Stash::prepare(PSTR("POST /api/tickets HTTP/1.1" 
                        "\r\n"
                        "Host: $F" "\r\n"
                        "User-Agent: Arduino/1.0" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "Content-Type: application/json" "\r\n"
                        "\r\n"
                        "$H"),
                        website, stash_size, sd);
    
    
    
    
    // send the packet - this also releases all stash buffers once done
    // Save the session ID so we can watch for it in the main loop.
    session = ether.tcpSend();
  }

   const char* reply = ether.tcpReply(session);
   if (reply != 0) {
     res = 0;
     if(reply[9] == '2'){
      Serial.println("Ticket válido.");
     }
     
     else if (reply[9] == '4'){
      Serial.println("Ticket usado ou não existente.");
     }
     else
      Serial.println("Algo deu errado!");
   }
   delay(50);
}



void initialize_ethernet(void){
  for(;;){ // keep trying until you succeed
    Serial.println("Reseting Ethernet...");
    
    // Change 'SS' to your Slave Select pin, if you arn't using the default pin
    if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0){
      Serial.println( "Failed to access Ethernet controller");
      continue;
    }

    if (!ether.dhcpSetup()){
      Serial.println("DHCP failed");
      continue;
    }

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    if (!ether.dnsLookup(website))
      Serial.println("DNS failed");

    ether.printIp("SRV: ", ether.hisip);
    //reset init value
    res = 0;
    break;
  }
}
