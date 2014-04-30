/* Internet of Things using Xively.com
   Multiple Data Stream Upload
   modified 30 Apr 2014
   by Mohamad Dani

  |-----       -----|
  |      RJ-45      |
  |    Connector    |
  |_________________|
   Wiz550io Module
      Top view
  -------------------
  J1               J2
  1- GND   |  *******
  2- GND   |  3.3V -1
  3- MOSI  |   RDY -2
  4- MISO  |   RST -3
  5- SLK   |    NC -4
  6- SS    |   INT -5
  7- 3.3V  |   GND -6
  8- 3.3V  |  *******
  --------------------
*/

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xAB, 0xCD, 0xEF, 0xEE, 0xEE, 0xEE};
#endif
IPAddress ip(192,168,1,10);

// Your Xively key to let you upload data
char xivelyKey[] = "YOUR API KEY"
#define xivelyFeed FEEDID

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int voltage = A0;
int energy = A1;
int humidity = A2;
int temperature = A3;

// Define the strings for our datastream IDs
char voltageID[]     = "Voltage";
char energyID[]      = "Energy";
char humidityID[]    = "Humidity";
char temperatureID[] = "Temperature";


XivelyDatastream datastreams[] =
{
  XivelyDatastream(voltageID, strlen(voltageID), DATASTREAM_FLOAT),
  XivelyDatastream(energyID, strlen(energyID), DATASTREAM_FLOAT),
  XivelyDatastream(humidityID, strlen(humidityID), DATASTREAM_FLOAT),
  XivelyDatastream(temperatureID, strlen(temperatureID), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, 4);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup()
{
  Serial.begin(9600);
  delay(1000);

  // start the Ethernet connection:
  // DHCP Setting
  if (Ethernet.begin()== 0)
  //If DHCP configuration failed
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to configure using IP address instead of DHCP:
    // start the Ethernet connection and the server:
    #if defined(WIZ550io_WITH_MACADDRESS)
      Ethernet.begin(ip);
    #else
      Ethernet.begin(mac, ip);
    #endif
  }

  Serial.print("Wiz5500io Module IP Address : "); Serial.println(Ethernet.localIP());
}

void loop()
{
  ///////////////////////////////////////////////////////
  //read sensor0 values
  int sensorValue = analogRead(voltage);
  sensorValue = map(sensorValue, 0, 1023, 0, 5);
  datastreams[0].setFloat(sensorValue);

  //print the sensor valye
  Serial.print("Read voltage (V) ");
  Serial.println(datastreams[0].getFloat());

  //send value to xively
  Serial.println("Uploading voltage value to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  //return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");
  delay(10);

   ///////////////////////////////////////////////////////
  //read sensor1 values
  sensorValue = analogRead(energy);
  sensorValue = map(sensorValue, 0, 1023, 0, 1000);
  datastreams[1].setFloat(sensorValue);

  //print the sensor valye
  Serial.print("Read Energy from Sensor 1 (W) ");
  Serial.println(datastreams[1].getFloat());

  //send value to xively
  Serial.println("Uploading Energy value to Xively");
  ret = xivelyclient.put(feed, xivelyKey);
  //return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");

  ///////////////////////////////////////////////////////
  //read sensor3 values
  sensorValue = analogRead(humidity);
  sensorValue = map(sensorValue, 0, 1023, 0, 100);
  datastreams[2].setFloat(sensorValue);

  //print the sensor 2 value
  Serial.print("Read Humidity (%H) ");
  Serial.println(datastreams[2].getFloat());

  //send value to xively
  Serial.println("Uploading voltage value to Xively");
  ret = xivelyclient.put(feed, xivelyKey);
  //return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");
  delay(10);

   ///////////////////////////////////////////////////////
  //read sensor3 values
  sensorValue = analogRead(temperature);
  sensorValue = map(sensorValue, 0, 1023, -10, 125);
  datastreams[3].setFloat(sensorValue);

  //print the sensor valye
  Serial.print("Read Temperature (C) ");
  Serial.println(datastreams[3].getFloat());

  //send value to xively
  Serial.println("Uploading Energy value to Xively");
  ret = xivelyclient.put(feed, xivelyKey);
  //return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");

  delay(10000);
}
