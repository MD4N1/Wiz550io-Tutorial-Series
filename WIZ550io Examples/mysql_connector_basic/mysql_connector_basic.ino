/* Demo menulis dan membaca data dari dan ke MySQL Database Server dari Arduino */
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet.h>
#include <stdlib.h>
#include <sha1.h>
#include <mysql.h>
//#include <avr/dtostrf.h>  // Add this for the Due

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
 byte mac[] = {0xAB, 0xCD, 0xEF, 0xEA, 0x0E, 0xEE};
#endif
IPAddress Arduino_ip(192,168,112,10);

IPAddress server_addr(192,168,112,1);

char user[] = "dodol";
char password[] = "dodol";

Connector my_conn; // The Connector/Arduino reference

const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Halo suster!', NULL)";
const char INSERT_DATA[] = "INSERT INTO test_arduino.temps VALUES (%s, NULL)";
const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

/*
 * do_query - execute a query and display results
 *
 * This method demonstrates how to execute a query, get the column
 * names and print them, then read rows printing the values. It
 * is a mirror of the show_results() example in the connector class.
 *
 * You can use this method as a template for writing methods that
 * must iterate over rows from a SELECT and operate on the values read.
 *
 */
void do_query(const char *q)
{
  column_names *c; // pointer to column values
  row_values *r;   // pointer to row values

  // First, execute query. If it returns a value pointer,
  // we have a result set to process. If not, we exit.
  if (!my_conn.cmd_query(q))
  {
    return;
  }

  // Next, we read the column names and display them.
  //
  // NOTICE: You must *always* read the column names even if
  //         you do not use them. This is so the connector can
  //         read the data out of the buffer. Row data follows the
  //         column data and thus must be read first.
  c = my_conn.get_columns();
  for (int i = 0; i < c->num_fields; i++)
  {
    Serial.print(c->fields[i]->name);
    if (i < c->num_fields - 1)
    {
      Serial.print(",");
    }
  }
  Serial.println();

  // Next, we use the get_next_row() iterator and read rows printing
  // the values returned until the get_next_row() returns NULL.
  int num_cols = c->num_fields;
  int rows = 0;
  do
  {
    r = my_conn.get_next_row();
    if (r)
    {
      rows++;
      for (int i = 0; i < num_cols; i++)
      {
        Serial.print(r->values[i]);
        if (i < num_cols - 1)
        {
          Serial.print(", ");
        }
      }
      Serial.println();
      // Note: we free the row read to free the memory allocated for it.
      // You should do this after you've processed the row.
      my_conn.free_row_buffer();
    }
  } while (r);
  Serial.print(rows);
  Serial.println(" rows in result.");

  // Finally, we are done so we free the column buffers
  my_conn.free_columns_buffer();
}

void setup()
{
  Serial.begin(38400);
  delay(10000);
  if (Ethernet.begin()== 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    #if defined(WIZ550io_WITH_MACADDRESS)
     Ethernet.begin(Arduino_ip);
    #else
     Ethernet.begin(mac, Arduino_ip);
    #endif
  }
   Serial.print("WIZ5500io Ethernet Module IP Address "); Serial.println(Ethernet.localIP());
   delay(1000);
   Serial.println("Connecting...");

  if (my_conn.mysql_connect(server_addr, 3306, user, password))
  {
    Serial.println("Arduino success to contact MySQL Database Server.");
    delay(1000);
  }
  else
    Serial.println("Arduino failed to contact MySQL Database Server.");

  //
  // Examples! What follows are examples of SELECT and INSERT statements that
  // demonstrate the most common operations you are likely to encounter.
  //
  // In order to run these examples, you must have the world sample database
  // installed () and the following databases and tables created:
  //
  // CREATE DATABASE test_arduino;
  // CREATE TABLE test_arduino.hello (msg char(50), msg_date timestamp);
  // CREATE TABLE temps (temp_c float, temp_date timestamp);

  //
  // SELECT Examples
  //
  // EXAMPLE 1
  // Inserting static text into a table.
  // Here we simply insert text data into a table. No conversion needed.
  // It also demonstrates the use of NULL to initiate a timestamp value.
  my_conn.cmd_query(INSERT_TEXT);
  // Now, let's check our results.
  do_query(HELLO_SQL);

  // EXAMPLE 2
  // Inserting real time data into a table.
  // Here we want to insert a row into a table but in this case we are
  // simulating reading the data from a sensor or some other component.
  // In this case, we 'simulate' reading temperature in celsius.
  float value_read = 26.9;
  // To use the value in an INSERT statement, we must construct a string
  // that has the value inserted in it. For example, what we want is:
  // 'INSERT INTO test_arduino.temps VALUES (26.9, NULL)' but the 26.9 is
  // held in the variable 'value_read'. So, we use a character string
  // formatting operation sprintf(). Notice here we must convert the float
  // to a string first and we use the %s specifier in the INSERT_DATA
  // string.
  char query[64];
  char temperature[10];
  dtostrf(value_read, 1, 1, temperature);
  sprintf(query, INSERT_DATA, temperature);
  my_conn.cmd_query(query);

  // Now, let's check our results.
  do_query(HELLO_DATA);
}

void loop()
{

}
