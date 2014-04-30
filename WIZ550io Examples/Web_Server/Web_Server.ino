/*
  Embedded Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Uno/Duemilanove and Wiz550io Ethernet Module

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 * Digital inputs attached to pins 2 through 13 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 12 Aug 2013
 by Soohwan Kim
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

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xAB, 0xCD, 0xEF, 0xEE, 0xEE, 0xEE};
#endif
IPAddress ip(192,168,1,10);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

// start the Ethernet connection and the server:
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip);
#else
  Ethernet.begin(mac, ip);
#endif
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino & Wiz550io Web Server Mohamad Dani</title>");
          client.println("<link rel=\"icon\" type=\"image/gif\" href=\"http://mamdgsm.webs.com/ico.gif\" />");
          client.println("<style type=\"text/css\">");
          client.println("body {background-color: #FFCC66;}");
          client.println("h1 {color: #990000; background-color: #FC9804;}");
          client.println("h4 {color: #990000; background-color: #FC9804;}");
          client.println("hr {border: 1; width: 20%; color: #FF0000; height: 5px;}");
          client.println("table {color: #333333; font-size: 11px; font-family: Verdana, Arial, Helvetica, sans-serif; font-weight: bold;}");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<center>");
          client.print("<h1>Embedded Web Server - Mohamad Dani 2014</h1>");
          client.print("<h4>Powered by Arduino & Wiz5500 Chip/Wiz550io Ethernet Module</h4>");
          client.println("<table width=20% border=\"0\">");

          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++)
          {
            int sensorReading = analogRead(analogChannel);
            client.print("<tr>");
            client.print("<td align=left width=50%><b>Analog I/O ");
            client.print(analogChannel);
            client.print("</b></td> <td align=right>");
            client.print(sensorReading);
            client.println("</td>");
            client.println("</tr>");
          }
          client.println("</table>");
          client.println("<hr />");
          client.println("<table width=20% border=\"0\">");
          for (int digitalChannel = 2; digitalChannel < 14; digitalChannel++)
          {
            int pinReading = digitalRead(digitalChannel);
            client.print("<tr>");
            client.print("<td align=left width=50%><b>Digital I/O ");
            client.print(digitalChannel);
            client.print("</b></td> <td align=right>");
            client.print(pinReading);
            client.println("</td>");
            client.println("</tr>");
          }
          client.println("</table>");
          client.println("</center>");
          client.println("</body>");
          client.println("</html>");
          break;
         }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    Serial.println("Client is disconnected");
  }
}

