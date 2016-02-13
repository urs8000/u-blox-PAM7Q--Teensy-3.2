// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

//This code is intended for use with Arduino Leonardo and other ATmega32U4-based Arduinos

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// set this to the hardware serial port you wish to use
// #define HWSERIAL Serial1

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using software serial (sketch example default):
//   Connect the GPS TX (transmit) pin to Digital 8
//   Connect the GPS RX (receive) pin to Digital 7
// If using hardware serial:
//   Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
//   Connect the GPS RX (receive) pin to matching TX1 (Digital 1)

// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
// SoftwareSerial mySerial(8, 7);
// Adafruit_GPS GPS(&mySerial);

// If using hardware serial, comment
// out the above two lines and enable these two lines instead:
Adafruit_GPS GPS(&Serial1);
 // HardwareSerial mySerial = Serial1;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true

void setup()  
{
    
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
   GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  delay(5000);
  // Ask for firmware version
  // mySerial.println(PMTK_Q_RELEASE);
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
   char c = GPS.read();
  // if you want to debug, this is a good time to do it!
   if ((c) && (GPSECHO))
     Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    // Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
                                        // the same as GPSECHE
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 5000) { 
    timer = millis(); // reset the timer
    
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.println();
      Serial.print("20");
      Serial.print(GPS.year, DEC);    Serial.print("/");
      Serial.print(GPS.month, DEC);   Serial.print("/");
      Serial.print(GPS.day, DEC);     Serial.print(" ");
      Serial.print(GPS.hour, DEC);    Serial.print(":");
      Serial.print(GPS.minute, DEC);  Serial.print(" ");
      Serial.print(GPS.latitude, 4);  Serial.print(GPS.lat); Serial.print(" ");
      Serial.print(GPS.longitude, 4); Serial.print(GPS.lon); Serial.print(" ");
      Serial.println();
      
      String year_s   = String(GPS.year);         Serial.print(year_s);
      String month_s  = String(GPS.month);        Serial.print(month_s);
      String day_s    = String(GPS.day);          Serial.print(day_s);      
      String hour_s   = String(GPS.hour);         Serial.print(hour_s);    // UTC 
      String minute_s = String(GPS.minute);       Serial.print(minute_s);
      Serial.println();

      // dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);   ....4725.778
      char lat_buf[11];
      dtostrf(GPS.latitude,10,3,lat_buf);
      Serial.println(lat_buf);

      char long_buf[11];
      dtostrf(GPS.longitude,10,3,long_buf);
      Serial.println(long_buf);

      String outbuf_s = "";
      outbuf_s = "20" + year_s + "/" + month_s + "/" + day_s + " "  + hour_s + ":" + minute_s + " " + lat_buf + "N " + long_buf + "E";
      Serial.println(outbuf_s);
      Serial.println(outbuf_s.length());
      
      // 1234567890123456789012345678901234567890
      // 2015/12/11 16:56   4725.790N    832.469E
      
    } // if GPSfix
  }   // timer abgelaufen (delay n millis)
}     // loop

/*
Time: 20:44:43.0
Date: 7/12/2015
Fix: 1 quality: 1
Location: 4725.6772N, 832.3679E
Speed (knots): 0.01
Angle: 249.82
Altitude: 301.60
Satellites: 7

9,E,0.05,249.82,071215,,,A*68
$PGTOP,11,2*6E
$GPGGA,204444.000,4725.6773,N,00832.3678,E,1,7,1.21,301.5,M,48.0,M,,*52
$GPRMC,204444.000,A,4725.6773,N,00832.3678,E,0.07,249.82,071215,,,A*6C
$PGTOP,11,2*6E
$GPGGA,204445.000,4725.6773,N,00832.3678,E,1,7,1.21,301.5,M,48.0,M,,*53
$GPRMC,204445.000,A,4725.6773,N,00832.3678,E,0.12,

------------------- no debug -------------------------------
Time: 10:13:29.0
Date: 9/12/2015
Fix: 1 quality: 0
Location: 4725.8007N, 832.4535E
Speed (knots): 1.63
Angle: 64.04
Altitude: 0.00
Satellites: 0

15/12/9 10:13 4725.8007N 832.4535E 


----------------- debug ---------------------------------------
GPGGA enabled : sends GGA & RMC
$GPGGA,122751.000,4725.7728,N,00832.4567,E,1,09,0.95,444.8,M,48.0,M,,*6C
$GPRMC,122751.000,A,4725.7728,N,00832.4567,E,0.88,18.06,111215,,,A*53

Time: 12:27:51.0
Date: 11/12/2015
Fix: 1 quality: 1
Location: 4725.7724N, 832.4567E
Speed (knots): 0.88
Angle: 18.06
Altitude: 444.80
Satellites: 9

GPRMC enabled :
$GPRMC,123125.000,A,4725.7755,N,00832.4431,E,0.20,289.91,111215,,,A*69
*/
