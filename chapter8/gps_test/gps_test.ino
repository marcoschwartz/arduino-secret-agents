// Libraries
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

// Pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// Instances
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup() {
  
  // Init board
  while (!Serial);
  Serial.begin(115200);
  Serial.println(F("FONA location test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }

  // Setup GPRS APN (username/password optional)
  fona.setGPRSNetworkSettings(F("internet"));
  //fona.setGPRSNetworkSettings(F("your_APN"), F("your_username"), F("your_password"));

  // Turn GPS on
  if (!fona.enableGPS(true)) {
    Serial.println(F("Failed to turn on GPS"));
  } 

  // Turn GPRS on
  fona.enableGPRS(true);

  // Decide between GPS or GPRS localisation
  boolean GPSloc;
  int8_t stat;
  
  // Check GPS fix
  stat = fona.GPSstatus();
  if (stat < 0) {
    GPSloc = false;
  }
  if (stat == 0 || stat == 1) {
    GPSloc = false;
  }
  if (stat == 2 || stat == 3) {
    GPSloc = true;
  }

  // Print which localisation method is used
  Serial.print("Localisation method: ");
  if (GPSloc) {Serial.println("GPS");}
  else {Serial.println("GPRS");}

  // Location variables
  String location;
  String latitude;
  String longitude;
  double latitudeNumeric;
  double longitudeNumeric;
 
  // Get location
  if (GPSloc) {
    location = getLocationGPS();
    latitude = getLatitudeGPS(location);
    longitude = getLongitudeGPS(location);
    latitudeNumeric = convertDegMinToDecDeg(latitude.toFloat());
    longitudeNumeric = convertDegMinToDecDeg(longitude.toFloat());
  }
  else {
    location = getLocationGPRS();
    latitude = getLatitudeGPRS(location);
    longitude = getLongitudeGPRS(location);
    latitudeNumeric = latitude.toFloat();
    longitudeNumeric = longitude.toFloat();
  }
  Serial.print("Latitude, longitude: ");
  Serial.print(latitudeNumeric, 6);
  Serial.print(",");
  Serial.println(longitudeNumeric, 6);
}

void loop() {
  // Nothing here
}

String getLocationGPRS() {

  // Buffer for reply & returncode
  char replybuffer[255];
  uint16_t returncode;

  // Get & return location
  if (!fona.getGSMLoc(&returncode, replybuffer, 250))
    return String("Failed!");
  if (returncode == 0) {
    return String(replybuffer);
  } else {
    return String(returncode);
  }
  
}

String getLocationGPS() {

  // Buffer
  char gpsdata[255];

  // Get data
  fona.getGPS(0, gpsdata, 255);
  return String(gpsdata);
}

String getLatitudeGPS(String data) {

  // Find commas
  int commaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', commaIndex+1);
  int thirdCommaIndex = data.indexOf(',', secondCommaIndex+1);
  
  return data.substring(commaIndex + 1, secondCommaIndex);
}


String getLongitudeGPS(String data) {

  // Find commas
  int commaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', commaIndex+1);
  int thirdCommaIndex = data.indexOf(',', secondCommaIndex+1);
  
  return data.substring(secondCommaIndex + 1, thirdCommaIndex);
}

String getLongitudeGPRS(String data) {

  // Find commas
  int commaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', commaIndex+1);

  return data.substring(0, commaIndex);
}

String getLatitudeGPRS(String data) {
  
  // Find commas
  int commaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', commaIndex+1);

  return data.substring(commaIndex + 1, secondCommaIndex);
}

double convertDegMinToDecDeg (float degMin) {
  double min = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );
 
  return decDeg;
}
