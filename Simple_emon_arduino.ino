/*
  Arduino & OpenEnergyMonitor 
  
  This sketch connects to an emoncms server and makes a request using
  Arduino Ethernet shield
  
  Author: Mirco Piccin aka pitusso
  Modified and simplified by J-F Payeur

  Tested on Arduino2560 + ethernet sheild (no sd card) 
  Tested on Arduino Ethernet (no sd card) 
  Tested on Arduino Uno +  ethernet sheild (no sd card) 
  
  based on    http://arduino.cc/en/Tutorial/WebClientRepeating
   
*/

#include <SPI.h>
#include <Ethernet.h>

  byte mac[] = {0x90, 0xA2, 0xDA, 0x04, 0x69, 0xD5};

  // fill in an available IP address on your network here, if dhcp auto config fail 
  IPAddress ip(192, 168, 81, 82);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress DNS(8, 8, 8, 8);
  IPAddress gw(192, 168, 81, 1);
  EthernetClient client;

 // Sensor pins
 const int lightSensorPin = A0;
 int lightValue =0 ;

//Emoncms configurations
//char server[] = "emoncms.org";      // name address for emoncms.org
// OR 
IPAddress server(192, 168, 81, 145);  // numeric IP for emoncms.org (no DNS)

  String apikey = "0fc3340c4434a7d42986e83fe2745e01";  //api key  emoncms.org
  int node = 1; //if 0, not used

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 5*1000;  // delay between updates, in milliseconds

void setup() {
 Serial.begin(9600);
 Serial.println("Emoncms client starting...");

  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("Failed to get an IP address using DHCP, forcing manually");
    Ethernet.begin(mac, ip, dns, gw, subnet);
  }

  pinMode(lightSensorPin, INPUT);           // set pin to input
  digitalWrite(lightSensorPin, HIGH);       // turn on pullup resistors
  
  printStatus();
}





void loop() {
  
  
  // if there's incoming data from the net connection.send it out the serial port.  This is for debugging purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("Disconnecting...");
    client.stop();
  }
  
  // if you're not connected, and at least <postingInterval> milliseconds have passed sinceyour last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
   
    //read sensors
    lightValue = analogRead(lightSensorPin);
    lightValue = map(lightValue, 50, 900, 100, 1);   //100 a 800 
    lightValue = constrain(lightValue, 0, 100);
    
      
    
    //Print values (debug)
    Serial.println();
    Serial.print(" ; Light : ");  Serial.print(lightValue);

    //send values
    sendData();
  }
  // store the state of the connection for next time through the loop:
  lastConnected = client.connected();
}


















// this method makes a HTTP connection to the server:
void sendData() {
         Serial.println("toti");
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP GET request:
    client.print("GET /emoncms/input/post?apikey=");
    client.print(apikey);
    if (node > 0) {
      client.print("&node=");
      client.print(node);
    }
    client.print("&json={");
    client.print("light:");
    client.print(lightValue);
    client.println("} HTTP/1.1");
    client.println("Host:emoncms.org");
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting...");
    client.stop();
  }
}


void printStatus() {
    // print your local IP address:
    Serial.print("IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }  

  Serial.println();
}

