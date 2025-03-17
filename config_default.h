#ifndef CONFIG_H
#define CONFIG_H

// set your WIFI credentials
const char* wifi_ssid   = YOUR_WIFI_SSID;
const char* wifi_pass   = YOUR_WIFI_PASS;

// set preferred time server
const char* ntp_server  = "europe.pool.ntp.org";

// add extra minute in TZ offsets for sending the correct time (the next minute will start at the end of the transmission) 
const char* timezone    = "CET-1:01CEST,M3.5.0,M10.5.0/3";   // Europe/Berlin

// comment this define to disable blinking the LED in sync with the transmitted pulses
#define BLINK

#endif
