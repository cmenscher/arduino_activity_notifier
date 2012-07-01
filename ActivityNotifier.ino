#include <SoftwareServo.h>
#include <WiServer.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

#define ledPin 2
#define servoPin 7

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,66};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"Intertrode Device"};		// max 32 bytes

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"makisupa"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 1
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 2
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00	// Key 3
								};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
//---------------------------------------------------------------------------


// Function that prints data from the server
void printData(char* data, int len) {
  
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and 
  // includes the HTTP header. 
  while (len-- > 0) {
    Serial.print(*(data++));
  } 
}

// Get the Findings stream
uint8 findings_ip[] = {50,16,203,11};
GETrequest getStream(findings_ip, 80, "findings.com", "/api/v1/timeline.xml");

SoftwareServo servo;
int updateInterval = 5000;  // delay between updates, in milliseconds
int jiggleInterval = 100;
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
unsigned long currentJiggle = 0;
unsigned long now = 0;
boolean initialRun = true;
boolean resetServo = false;


void setup()
{
        // start serial port:
        Serial.begin(57600);
        // give the ethernet module time to boot up:
        delay(1000);

        WiServer.init(NULL);
        WiServer.enableVerboseMode(true);
        getStream.setReturnFunc(printData);
        
        pinMode(ledPin, OUTPUT);
        pinMode(servoPin, OUTPUT);

        servo.attach(servoPin);
  
        Serial.println("Initializing WiFi...");
  	//WiFi.init();
        Serial.println("Ready...........FIGHT!");
        //give notice that we're up and running
        digitalWrite(ledPin, LOW);
}

void loop() {
  if(initialRun || (millis() - lastConnectionTime > updateInterval)) {
    Serial.println("Getting Findings feed...");
    getStream.submit();
    //webclient_get(findings_ip, 80, "/api/v1/timeline.xml");
    
    lastConnectionTime = millis();
    if(initialRun) {
      initialRun = false;
    }
  }
  	
  if(resetServo) {
    //servo.write(0);
    digitalWrite(ledPin, LOW);
  } else {
    //servo.write(100);
    digitalWrite(ledPin, HIGH);
  }
  
  now = millis();
  //  Serial.print(now);
  //  Serial.print("-");
  //  Serial.print(currentJiggle);
  //  Serial.print(" = ");
  //  Serial.println(now-currentJiggle);      
  if(now - currentJiggle > jiggleInterval) {
    Serial.println("moving servo.......................");
    currentJiggle = millis();
    resetServo = !resetServo;
  }

  //WiFi.run();
  SoftwareServo::refresh();
  // Run WiServer
  WiServer.server_task();
 
  delay(10);
}
