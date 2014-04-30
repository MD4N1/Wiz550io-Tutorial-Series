/*
 Twitter Sensor Network

 SimplePost messages to Twitter (tweet) from Aruduino with Ethernet Shield!

 Notice
 -The library uses this site as a proxy server for OAuth stuff. Your tweet may not be applied during maintenance of this site.
 -Please avoid sending more than 1 request per minute not to overload the server.
 -Twitter seems to reject repeated tweets with the same contenet (returns error 403).

 Reference & Lincense
 URL : http://arduino-tweet.appspot.com/

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 2011
 by NeoCat - (ver1.3 - Support IDE 1.0)
 tested &
 modified 13 Aug 2013
 by Soohwan Kim - (ver1.3 - Support IDE 1.0.5)
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
#include <Twitter.h> // Twitter.h in library/Ethernet folder. @diffinsight 2013-08-19

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xAB, 0xCD, 0xEF, 0xEE, 0xEE, 0xEE};
#endif
IPAddress ip(192,168,1,10);

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/) //Token for @MD4N1
Twitter twitter("Your Token Here");

// Message to post
char pesan[] = "Arduino akan mentweet data sensor-sensor rumah saya";

// Variabel
int tegangan, energi, kelembaban, temperatur;
char datanya[140];

void tweet(char msg[])
{
  Serial.println("menghubungkan ...");
  if (twitter.post(msg))
  {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200)
    {
      Serial.println("OK.");
    }
    else
    {
      Serial.print("gagal : kode ");
      Serial.println(status);
    }
  }
  else
  {
    Serial.println("Koneksi gagal.");
  }
}

void tweet_sensor()
{
 tegangan = analogRead(A0);
 tegangan = map(tegangan, 0, 1023, 0, 5000);

 energi = analogRead(A1);
 energi = map(energi, 0, 1023, 0, 1000);

 kelembaban = analogRead(A2);
 kelembaban = map(kelembaban, 0, 1023, 0, 100);

 temperatur = analogRead(A3);
 temperatur = map(temperatur, 0, 1023, 0, 100);

 sprintf(datanya, "Energi %d KWH, Kelembaban %d percent%", energi, kelembaban);
 tweet(datanya);

 delay(10);
 sprintf(datanya, "Tegangan %d mV, Suhu %d C", tegangan, temperatur);
 tweet(datanya);
}

void setup()
{
  delay(5000);
  Serial.begin(9600);
  // start the Ethernet connection:
  // DHCP Setting
  if (Ethernet.begin() == 0)
  //If DHCP configuration failed
  {
    Serial.println("Gagal mengkonfigurasi Ethernet menggunakan DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to configure using IP address instead of DHCP:
    // start the Ethernet connection and the server:
    #if defined(WIZ550io_WITH_MACADDRESS)
        Ethernet.begin(ip);
    #else
        Ethernet.begin(mac, ip);
    #endif
  }

  Serial.print("Alamat IP Modul Wiz5500io : "); Serial.println(Ethernet.localIP());
  Serial.println("menghubungkan ...");
  tweet(pesan);
}

void loop()
{
    delay(60000);
    tweet_sensor();
}

