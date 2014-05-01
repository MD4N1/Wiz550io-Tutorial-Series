/*
  Example queries for using the Connector/Arduino library.

  This file contains a number of examples as discussed in the Readme. It
  also contains an example of how to use the library with the WiFi shield.

  In order to run these examples, you must have the world sample database
  installed (http://dev.mysql.com/doc/index-other.html) and the following
  databases and tables created:

  CREATE DATABASE test_arduino;
  CREATE TABLE test_arduino.hello (msg char(50), msg_date timestamp);
  CREATE TABLE temps (temp_c float, temp_date timestamp);

  Take some time to read through the code before you load and run it.

  NOTICE: There are a lot of queries in this file. Together they use a lot
          of program space - especially the dtostrf() example. If you attempt
          to run all of these at one time, depending on your board you may
          run out of space or the sketch may run out of memory and hang.
          Thus, if you want to run these tests, I recommend doing them
          *one at a time*.

          Because of this, all of the examples are commented out. To run one,
          just uncomment it and its corresponding string constant at the
          top of the file and off you go! :)
*/
#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
//#include <avr/dtostrf.h>  // Add this for the Due if you need drostrf
#include <stdlib.h>
//#include <WiFi.h>  // Use this for WiFi
#include <mysql.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(10,0,1,13);  // Supply the IP of the MySQL *server* here

char user[] = "root";         // can be anything but the user must have
char password[] = "secret";   // access rights to connect (host must permit it)

// WiFi card example
//char ssid[] = "my_lonely_ssid";
//char pass[] = "horse_no_name";

Connector my_conn;        // The Connector/Arduino reference

// String constants for examples. Uncomment those you need.

//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
//const char INSERT_DATA[] = "INSERT INTO test_arduino.temps VALUES (%s, NULL)";
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

/**
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
void do_query(const char *q) {
  column_names *c; // pointer to column values
  row_values *r;   // pointer to row values

  // First, execute query. If it returns a value pointer,
  // we have a result set to process. If not, we exit.
  if (!my_conn.cmd_query(q)) {
    return;
  }

  // Next, we read the column names and display them.
  //
  // NOTICE: You must *always* read the column names even if
  //         you do not use them. This is so the connector can
  //         read the data out of the buffer. Row data follows the
  //         column data and thus must be read first.

  c = my_conn.get_columns();
  for (int i = 0; i < c->num_fields; i++) {
    Serial.print(c->fields[i]->name);
    if (i < c->num_fields - 1) {
      Serial.print(",");
    }
  }
  Serial.println();

  // Next, we use the get_next_row() iterator and read rows printing
  // the values returned until the get_next_row() returns NULL.

  int num_cols = c->num_fields;
  int rows = 0;
  do {
    r = my_conn.get_next_row();
    if (r) {
      rows++;
      for (int i = 0; i < num_cols; i++) {
        Serial.print(r->values[i]);
        if (i < num_cols - 1) {
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

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  Ethernet.begin(mac_addr);

  // WiFi section
//  int status = WiFi.begin(ssid, pass);
//  // if you're not connected, stop here:
//  if ( status != WL_CONNECTED) {
//    Serial.println("Couldn't get a wifi connection");
//    while(true);
//  }
//  // if you are connected, print out info about the connection:
//  else {
//    Serial.println("Connected to network");
//    IPAddress ip = WiFi.localIP();
//    Serial.print("My IP address is: ");
//    Serial.println(ip);
//  }

  delay(1000);
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");

  //
  // SELECT Examples
  //

/*
  // EXAMPLE 1

  // SELECT query returning rows (built-in methods)
  // Here we simply read the columns, print the names, then loop through
  // the rows printing the values read. We set a limit to make this something
  // that executes in a reasonable timeframe.

  my_conn.cmd_query(TEST_SELECT_QUERY);
  my_conn.show_results();
*/
/*
  // EXAMPLE 2

  // SELECT query returning rows (custom method)
  // Here we execute the same query as above but use a custom method for reading
  // and displaying the results. See the do_query() method above for more
  // information about how it works.

  do_query(TEST_SELECT_QUERY);
*/
/*
  // EXAMPLE 3

  // SELECT query for lookup value (1 row returned)
  // Here we get a value from the database and use it.

  long head_count = 0;
  my_conn.cmd_query(QUERY_POP);

  // We ignore the columns but we have to read them to get that data out of the queue

  my_conn.get_columns();

  // Now we read the rows.

  row_values *row = NULL;
  do {
    row = my_conn.get_next_row();
    // We use the first value returned in the row - population of NYC!
    if (row != NULL) {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);

  // We're done with the buffers so Ok to clear them (and save precious memory).

  my_conn.free_columns_buffer();
  my_conn.free_row_buffer();

  // Now, let's do something with the data.

  Serial.print("NYC pop = ");
  Serial.println(head_count);
*/

  //
  // INSERT Examples
  //

/*
  // EXAMPLE 4

  // Inserting static text into a table.
  // Here we simply insert text data into a table. No conversion needed.
  // It also demonstrates the use of NULL to initiate a timestamp value.

  my_conn.cmd_query(INSERT_TEXT);
  // Now, let's check our results.
  do_query(HELLO_SQL);
*/
/*
  // EXAMPLE 5

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
*/
}

void loop() {
}
