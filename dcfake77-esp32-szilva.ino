// dcfake77-esp32
// Copyright (C) 2018-2023  Luigi Calligaris
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <WiFi.h>
#include "time.h"
#include "settime.h"

#include "dcf77protocol.h"


#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

const char* wifi_ssid   = "szilvanetp";
const char* wifi_pass   = "smcbarricade7";

const char* ntp_server  = "europe.pool.ntp.org";
// add extra minute in TZ offsets for sending the correct time (the next minute will start at the end of the transmission) 
const char* timezone    = "CET-1:01CEST,M3.5.0,M10.5.0/3";   // Europe/Berlin
//const char* timezone    = "BST-0:01GMT,M3.2.0/2:00:00,M11.1.0/2:00:00";  // UK/GMT
//const char* timezone    = "EST5EDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // USA/New York

const unsigned led_pwm_freq       = 77490;
const unsigned led_pwm_channel    =     0;
const unsigned led_pwm_pin        =    16;
const unsigned led_pwm_resolution =     5;
const unsigned led_pwm_duty_off   =     2;
const unsigned led_pwm_duty_on    =    16;

static struct tm  local_time;
static uint8_t    dcf77_one_minute_data[60];


void PrintLocalTime()
{
	Serial.print(&local_time, "%A, %B %d %Y %H:%M:%S %Z ");
	Serial.println(local_time.tm_isdst ? "DST" : "   ");
}

void WaitNextSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	delayMicroseconds(1000000-tv.tv_usec);
}

void setup()
{
	Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

	//connect to WiFi
	Serial.printf("Connecting to %s ", wifi_ssid);
	WiFi.begin(wifi_ssid, wifi_pass);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println(" CONNECTED");
	
	//init and get the time
	while (1)
	{

    // to debug daylight saving switch  
    //setTime(2021,10,31,0,58,50,1);  
    //setTimezone(timezone);
  	
    // to run in normal mode
    configTzTime(timezone, ntp_server);
		
		if (!getLocalTime(&local_time))
			Serial.println("Failed to obtain time");
		else
			break;
		
		delayMicroseconds(100000);
	}
	
	Serial.println("Initialized time");
	
	PrintLocalTime();

	// Disconnect WiFi as it's no longer needed
	WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);
	
	// Configure LED PWM functionalitites
	//ledcSetup(led_pwm_channel, led_pwm_freq, led_pwm_resolution);
	// Attach the channel to the GPIO to be controlled
	//ledcAttachPin(led_pwm_pin, led_pwm_channel);
	
	ledcAttachChannel(led_pwm_pin, led_pwm_freq, led_pwm_resolution, led_pwm_channel);
	
	// Initialize the vector holding the values for the DCF77 encoding
	if(!getLocalTime(&local_time))
		Serial.println("Failed to obtain time");
	dcf77_encode_data(&local_time, dcf77_one_minute_data);
}

void loop()
{
	WaitNextSec();
	
	if(!getLocalTime(&local_time))
		Serial.println("Failed to obtain time");
	
	if (local_time.tm_sec > 58)
	{
		//ledcWrite(led_pwm_channel, led_pwm_duty_on);
		ledcWriteChannel(led_pwm_channel, led_pwm_duty_on);
	}
	else
	{
    //ledcWrite(led_pwm_channel, led_pwm_duty_off);
    ledcWriteChannel(led_pwm_channel, led_pwm_duty_off);
    digitalWrite(LED_BUILTIN, HIGH);

		if (dcf77_one_minute_data[local_time.tm_sec] == 0)
			delayMicroseconds(100000); //100 ms = 0
		else
			delayMicroseconds(200000); //200 ms = 1

    //ledcWrite(led_pwm_channel, led_pwm_duty_on);
    ledcWriteChannel(led_pwm_channel, led_pwm_duty_on);
    digitalWrite(LED_BUILTIN, LOW);
			
	}
	
	if (local_time.tm_sec == 0)
		dcf77_encode_data(&local_time, dcf77_one_minute_data);
	
	PrintLocalTime();
}
