// Robot test via aREST + WiFi
#define NUMBER_VARIABLES 1
#define NUMBER_FUNCTIONS 5

// Libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <aREST.h>
#include <avr/wdt.h>

// CC3000 pins
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  8
#define ADAFRUIT_CC3000_CS    10

// Robot speed
#define FULL_SPEED 100
#define TURN_SPEED 50

// Motor pins
int speed_motor1 = 6;  
int speed_motor2 = 5;
int direction_motor1 = 7;
int direction_motor2 = 4;

// CC3000 instance
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT);

// Create aREST instance
aREST rest = aREST();

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

// Server instance
Adafruit_CC3000_Server restServer(LISTEN_PORT);

#define WLAN_SSID       "KrakowskiePrzedm51m.15(flat15)"
#define WLAN_PASS       "KrK51flat15_1944_15"
#define WLAN_SECURITY   WLAN_SEC_WPA2

void setup(void)
{  
  // Start Serial
  Serial.begin(115200);
  
  // Give name to robot
  rest.set_id("1");
  rest.set_name("robot");
  
  // Expose functions
  rest.function("forward",forward);
  rest.function("backward",backward);
  rest.function("left",left);
  rest.function("right",right);
  rest.function("stop",stop);
  
  // Set up CC3000 and get connected to the wireless network.
  Serial.print(F("Initialising CC3000..."));
  if (!cc3000.begin())
  {
    while(1);
  }
  Serial.println(F("done"));

  Serial.print(F("Connecting to WiFi..."));
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    while(1);
  }
  Serial.println(F("done"));

  Serial.print(F("Getting DHCP..."));
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
   
  // Start server
  restServer.begin();
  Serial.println(F("Listening for connections..."));
  
  displayConnectionDetails();
 
  wdt_enable(WDTO_8S);
}

void loop() {  
 
  // Handle REST calls
  Adafruit_CC3000_ClientRef client = restServer.available();
  rest.handle(client);
  wdt_reset();
  
  // Check connection
  if(!cc3000.checkConnected()){while(1){}}
  wdt_reset(); 
}

// Forward
int forward(String command) {
  
  send_motor_command(speed_motor1,direction_motor1,100,1);
  send_motor_command(speed_motor2,direction_motor2,100,1);
  return 1;
}

// Backward
int backward(String command) {
  
  send_motor_command(speed_motor1,direction_motor1,100,0);
  send_motor_command(speed_motor2,direction_motor2,100,0);
  return 1;
}

// Left
int left(String command) {
  
  send_motor_command(speed_motor1,direction_motor1,75,0);
  send_motor_command(speed_motor2,direction_motor2,75,1);
  return 1;
}

// Right
int right(String command) {
  
  send_motor_command(speed_motor1,direction_motor1,75,1);
  send_motor_command(speed_motor2,direction_motor2,75,0);
  return 1;
}

// Stop
int stop(String command) {
  
  send_motor_command(speed_motor1,direction_motor1,0,1);
  send_motor_command(speed_motor2,direction_motor2,0,1);
  return 1;
}

// Function to command a given motor of the robot
void send_motor_command(int speed_pin, int direction_pin, int pwm, boolean dir)
{
  analogWrite(speed_pin,pwm); // Set PWM control, 0 for stop, and 255 for maximum speed
  digitalWrite(direction_pin,dir);
}

// Print connection details of the CC3000 chip
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
