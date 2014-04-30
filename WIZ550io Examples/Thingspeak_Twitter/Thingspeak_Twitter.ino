/*

 ThingSpeak Client to Update Channel Feeds and Twitter Status

 The ThingSpeak Client + Twitter sketch is designed for the Arduino + Ethernet
 and tested with the Arduino 0022 IDE.

 This sketch updates a ThingSpeak Channel and also updates a Twitter status.
 Channel feeds are updated via the ThingSpeak API (http://community.thingspeak.com/documentation/)
 using HTTP POST.

 Twitter status are updated via the ThingTweet App
 (http://community.thingspeak.com/documentation/apps/thingtweet/) using HTTP POST.
 ThingTweet is a Twitter proxy web application that handles the OAuth.

 Getting Started with ThingSpeak and ThingTweet:

    * Sign Up for New User Account - https://www.thingspeak.com/users/new
    * Create a New Channel by selecting Channels and then Create New Channel
    * Enter the Write API Key in this sketch under "ThingSpeak Settings"
    * Link your Twitter account to the ThingTweet App - Apps / ThingTweet
    * Enter the ThingTweet API Key in this sketch under "ThingSpeak Settings"

 Created: July 24, 2011 by Hans Scharler (http://www.iamshadowlord.com)

*/
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,112,10);

// ThingSpeak Settings
byte server[]  = { 184, 106, 153, 149 };        // IP Address for the ThingSpeak API
String writeAPIKey      = "Thinkspeak Channnel API";   // Write API Key for a ThingSpeak Channel
String thingtweetAPIKey = "thingtweetAPIKey";   // Write API Key for a ThingSpeak Channel
const int updateThingSpeakInterval = 30000;     // Time interval in milliseconds to update ThingSpeak (30000 = 30 seconds)
const long updateTwitterInterval = 60000;      // Time interval in milliseconds to update Twitter (120000 = 2 minute)
EthernetClient client;

// Variable Setup
long lastConnectionTime = 0;
long lastTweetTime = 0;
boolean lastConnected = false;
int resetCounter = 0;

// Variabel
unsigned int tegangan, energi, kelembaban, temperatur;

void resetEthernetShield()
{
  Serial.println("Resetting Ethernet Shield.");
  Serial.println();
  client.stop();
  delay(1000);
  if (Ethernet.begin()== 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
   // DHCP failed, so use a fixed IP address:
   #if defined(WIZ550io_WITH_MACADDRESS)
     Ethernet.begin(ip);
   #else
     Ethernet.begin(mac, ip);
   #endif
   }
    Serial.print("WIZ5500io Ethernet Module IP Address "); Serial.println(Ethernet.localIP());
    delay(1000);
}
void updateThingSpeak(String tsData)
{
  if (client.connect(server,80))
  {
    Serial.println("Connected to ThingSpeak...");
    Serial.println();

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);

    lastConnectionTime = millis();

    resetCounter = 0;

  }
  else
  {
    Serial.println("Connection Failed.");
    Serial.println();

    resetCounter++;

    if (resetCounter >=5 ) {resetEthernetShield();}

    lastConnectionTime = millis();
  }
}

void updateTwitterStatus(String tsData)
{
  if (client.connect(server,80) && tsData.length() > 0)
  {
    // Create HTTP POST Data
    tsData = "api_key="+thingtweetAPIKey+"&status="+tsData;

    Serial.println("Connected to ThingTweet...");
    Serial.println();

    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);

    lastTweetTime = millis();
  }
  else
  {
    Serial.println("Connection Failed.");
    Serial.println();

    lastTweetTime = millis();
  }
}

void setup()
{
  Serial.begin(9600);
  delay(10000);
  if (Ethernet.begin()== 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
   // DHCP failed, so use a fixed IP address:
   #if defined(WIZ550io_WITH_MACADDRESS)
     Ethernet.begin(ip);
   #else
     Ethernet.begin(mac, ip);
   #endif
   }
   Serial.print("WIZ5500io Ethernet Module IP Address "); Serial.println(Ethernet.localIP());

   delay(1000);
   updateTwitterStatus("Arduino belajar bersiul, maklum mikrokontroller juga ingin bisa ngetwit sama halnya manusia");
}

void loop()
{
 tegangan = analogRead(A0);
 tegangan = map(tegangan, 0, 1023, 0, 5000);

 energi = analogRead(A1);
 energi = map(energi, 0, 1023, 0, 1000);

 kelembaban = analogRead(A2);
 kelembaban = map(kelembaban, 0, 1023, 0, 100);

 temperatur = analogRead(A3);
 temperatur = map(temperatur, 0, 1023, 0, 100);

 String tegangan1 = String(tegangan, DEC);
 String energi1   = String(energi, DEC);
 String kelembaban1 = String(kelembaban, DEC);
 String temperatur1 = String(temperatur, DEC);

  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("...disconnected.");
    Serial.println();

    client.stop();
  }

  // Update ThingSpeak Channel
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    updateThingSpeak("field1="+tegangan1+"&field2="+energi1+"&field3="+kelembaban1+"&field4="+temperatur1);
    //updateTwitterStatus("Tegangan: "+tegangan1+"mV "+"Energi: "+energi1+"KWH "+"Kelembaban: "+kelembaban1+"% "+"Suhu: "+temperatur1+"C");
  }

  // Update Twitter Status via ThingTweet
  if(!client.connected() && (millis() - lastTweetTime > updateTwitterInterval))
  {
    updateTwitterStatus("Tegangan: "+tegangan1+"mV "+"Energi: "+energi1+"KWH "+"Suhu: "+temperatur1+" C");
  }

  lastConnected = client.connected();
}
