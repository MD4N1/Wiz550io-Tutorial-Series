/**
  Copyright (c) 20012, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

  Example: DHT-22 Sensor Node

  This code module demonstrates a basic data collection node in the form
  of a temperature and humidity sensor node. It uses the common DHT-22
  sensor connected to an Arduino with an Ethernet shield.

  You will need the Connector/Arduino library along with the DHT22 library:

  https://github.com/ringerc/Arduino-DHT22

  The DHT-22 is connected as follows:

    Pin      Connected to
     1       +5v, 4.7K resistor between VCC and the data pin (strong pullup)
     2       pin 7 on Arduino, 4.7k resistor
     3       no connection
     4       ground

  See the excellent tutorial on DHT-22 at:
  
  http://www.ladyada.net/learn/sensors/dht.html

  You will also need to create a table on your MySQL server as
  follows (modify it to suite your needs but if you do modify the
  query statement in the code as well):

  CREATE TABLE `test`.`temp` (
    `id` int(11) NOT NULL AUTO_INCREMENT,
    `temp_c` float DEFAULT NULL,
    `rel_humid` float DEFAULT NULL,
    PRIMARY KEY (`id`)
  ) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=latin1

  Other changes needed:
    - change the ip_addr below to the address of your Arduino
    - change the server_addr below to the address of your MySQL server
    - change user, password to the user and password for your MySQL server\
    - change read_delay to your chosen sample rate

  Possible Mods:
    - change the code to store temp in Fahrenheit
      or change the table to automatically convert to Fahrenheit thereby
      moving processing from the Arduino to the database - cool!
*/
#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <avr/pgmspace.h>
#include "mysql.h"
#include <DHT22.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip_addr(192, 168, 0, 15);
IPAddress server_addr(192, 168, 0, 7); 
char user[] = "root";
char password[] = "root";

Connector my_conn;        // The Connector/Arduino reference

#define DHT22_PIN 7       // DHT2 data is on pin 7
#define read_delay 5000   // 5 seconds 
DHT22 myDHT22(DHT22_PIN); // DHT22 instance

void read_data() {
  DHT22_ERROR_t errorCode;
  
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      char buf[128];
      sprintf(buf, "INSERT INTO test.temp VALUES (NULL, %hi.%01hi, %i.%01i)",
                   myDHT22.getTemperatureCInt()/10,
                   abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10,
                   myDHT22.getHumidityInt()%10);
      my_conn.cmd_query(buf);
      Serial.println("Data read and recorded.");
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled too quick ");
      break;
  }
}

void setup() {  
  Ethernet.begin(mac_addr);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_addr, 3306, user, password))
    delay(500);
  else
    Serial.println("Connection failed.");
}

void loop() {  
  delay(read_delay);
  read_data();
}
