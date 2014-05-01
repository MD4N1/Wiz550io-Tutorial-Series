/*
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

  mysql.h - Library for communicating with a MySQL Server over Ethernet.

  This header file defines the Connector class for connecting to and
  issuing queries against a MySQL database. You can issue any command
  using SQL statements for inserting or retrieving data.

  Dependencies:

    - requires the SHA1 code from google code repository. See README.txt
      for more details.

  Version 1.0.0a Created by Dr. Charles A. Bell, April 2012.
  Version 1.0.0b Updated by Dr. Charles A. Bell, October 2013.
*/
#ifndef mysql_h
#define mysql_h

#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>

#define WITH_SELECT  // Comment out this for use without SELECT capability
                     // to save space.

//#define WIFI       // Uncomment out this for use with the WiFi shield
//#include <WiFi.h>  // Uncomment out this for use with the WiFi shield

#define OK_PACKET     0x00
#define EOF_PACKET    0xfe
#define ERROR_PACKET  0xff
#define MAX_FIELDS    0x20   // Maximum number of fields. Reduce to save memory. Default=32
#define VERSION_STR   "1.0.0b"

// Structure for retrieving the OK packet.
typedef struct {
  int affected_rows;
  int insert_id;
  int server_status;
  int warning_count;
  char message[64];
} ok_packet;

// Structure for retrieving the EOF packet.
typedef struct {
  int warnings;
  int flags;
} eof_packet;

#if defined WITH_SELECT

// Structure for retrieving a field (minimal implementation).
typedef struct {
  char *db;
  char *table;
  char *name;
} field_struct;

// Structure for storing result set metadata.
typedef struct {
  int num_fields;     // actual number of fields
  field_struct *fields[MAX_FIELDS];
} column_names;

// Structure for storing row data.
typedef struct {
  char *values[MAX_FIELDS];
} row_values;

#endif

/**
 * Connector class
 *
 * The connector class permits users to connect to and issue queries
 * against a MySQL database. It is a lightweight connector with the
 * following features.
 *
 *  - Connect and authenticate with a MySQL server (using 'new' 4.1+
 *    protocol).
 *  - Issue simple commands like INSERT, UPDATE, DELETE, SHOW, etc.
 *  - Run queries that return result sets.
 *
 *  There are some strict limitations:
 *
 *  - Queries must fit into memory. This is because the class uses an
 *    internal buffer for building data packets to send to the server.
 *    It is suggested long strings be stored in program memory using
 *    PROGMEM (see cmd_query_P).
 *  - Result sets are read one row-at-a-time.
 *  - The combined length of a row in a result set must fit into
 *    memory. The connector reads one packet-at-a-time and since the
 *    Arduino has a limited data size, the combined length of all fields
 *    must be less than available memory.
 *  - Server error responses are processed immediately with the error
 *    code and text written via Serial.print.
 */
class Connector
{
  public:
    Connector();
    boolean mysql_connect(IPAddress server, int port,
                          char *user, char *password);
    boolean cmd_query(const char *query);
    boolean cmd_query_P(const char *query);
    int is_connected () { return client.connected(); }
    const char *version() { return VERSION_STR; }
#if defined WITH_SELECT
    column_names *get_columns();
    row_values *get_next_row();
    void free_columns_buffer();
    void free_row_buffer();
    void show_results();
#endif
  private:
    byte *buffer;
    char *server_version;
    byte seed[20];
    int packet_len;
#if defined WITH_SELECT
    column_names columns;
    boolean columns_read;
    int num_cols;
    row_values row;
#endif

    // Determine if WiFi shield is used
    #if defined WIFI
      WiFiClient client;
    #else
      EthernetClient client;
    #endif

    // Methods for handling packets
    int wait_for_client();
    void send_authentication_packet(char *user, char *password);
    void read_packet();
    void parse_handshake_packet();
    void parse_eof_packet(eof_packet *packet);
    int parse_ok_packet(ok_packet *packet);
    void parse_error_packet();
    boolean run_query(int query_len);

    // Utility methods
    boolean scramble_password(char *password, byte *pwd_hash);
    int get_lcb_len(int offset);
    char *read_string(int *offset);
    int read_int(int offset, int size=0);
    void store_int(byte *buff, long value, int size);
#if defined WITH_SELECT
    int get_field(field_struct *fs);
    int get_row();
    boolean get_fields();
    boolean get_row_values();
    column_names *query_result();

    // diagnostic methods
    void print_packet();
#endif
};

#endif
