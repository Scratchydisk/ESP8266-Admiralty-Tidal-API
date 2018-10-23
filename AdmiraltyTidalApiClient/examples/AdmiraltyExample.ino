/*
 Name:		AdmiraltyTidalApiClient.h
 Created:	10/21/2018 10:43:30 AM
 Author:	Stewart McSporran
 Editor:	http://www.visualmicro.com

 An example that shows how to retrieve tidal event forecasts for
 a UK port and print out the results to the serial port.

 You will need to register for a developer account (free)
 at https://admiraltyapi.portal.azure-api.net

 Library hosted at https://github.com/Scratchydisk/ESP8266-Admiralty-Tidal-API

 Released under the MIT license.

Copyright (c) 2018 Stewart McSporran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Configuration.h"

#include "AdmiraltyTidalApiClient.h"
#include <JsonStreamingParser.h>
#include <JsonListener.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <TimeLib.h>

/**********************************************
 * User Configuration Section
 **********************************************/

 // Supply your own WiFi's SSID and password

const char* _ssid = "<ssid>";
const char* _pwd = "<pwd>";

// Enter your Tidal API key here (from the profile page of your API account)
String ADMIRALTY_AUTH_SUBSCRIPTION_KEY = "<key>";

// The ID of the tidal station to get the forecasts for.
// See https://admiraltyapi.portal.azure-api.net/docs/services/uk-tidal-api/operations/Stations_GetStations?
// for the API call to retrieve these IDs.
// The example ID here is for my local port, Ayr in Scotland.
String ADMIRALTY_STATION_ID = "0413";

/**********************************************
 * End User Configuration Section
 **********************************************/


WiFiClientSecure _wifiClient;

AdmiraltyApiClient _tidalApiClient;

// Utility for output, adds leading 0 to numbers < 10
String leadingDigit(uint8_t value)
{
	if (value < 10)
	{
		return "0" + String(value);
	}
	else
	{
		return String(value);
	}
}

void setup()
{
	// Initialise the API client's subscription key and station ID
	_tidalApiClient.apiSubscriptionKey = ADMIRALTY_AUTH_SUBSCRIPTION_KEY;
	_tidalApiClient.stationId = ADMIRALTY_STATION_ID;

	// We'll validate the SSL certificate against the thumbprint stored in
	// the library (NB: that certificate expires in Jan 2020)
	_tidalApiClient.validateSslThumbprint = true;

	/*** Initialise the WiFi connection ***/
	Serial.println("Setting up WiFi");

	// Disconnect from existing WiFI
	//WiFi.mode(WIFI_STA);
	//WiFi.disconnect();
	//delay(100);

	// Connect to the WiFi network
	Serial.print("Connecting to ");
	Serial.print(_ssid);
	WiFi.begin(_ssid, _pwd);

	// Wait for connection
	while (!WiFi.isConnected())
	{
		Serial.print(".");
		delay(500);
	}
	Serial.println("Connected");
	// Print the local IP
	Serial.print("IP address : ");
	Serial.println(WiFi.localIP());

	/*** Fetch 2 days' forecasts, parse them and print to the serial monitor ***/

	// Fetch 2 days' forecasts
	int result = _tidalApiClient.FetchTidalEvents(_wifiClient, 2);

	if (result == ADMIRALTY_API_HOSTBYNAME_FAILED)
	{
		Serial.println("Failed to resolve the API's hostname");
		return;
	}
	else if (result == ADMIRALTY_API_CONNECT_FAILED)
	{
		Serial.println("Unable to create https connection to API host");
		return;
	}
	else if (result == ADMIRALTY_API_SSL_VALIDATION_FAILED)
	{
		Serial.println("API host SSL validation failed");
		return;
	}

	Serial.println("===== Fetched " + String(_tidalApiClient.numberEvents) + " Events =====");

	for (int i = 0; i < _tidalApiClient.numberEvents; i++)
	{
		// Print out the i'th forecast event in the format
		// Year-Month-Day Hour:Minute (Low|High) tide of (height)m

		TidalEvent te = _tidalApiClient.tidalEvents[i];

		String tideType = te.isHighTide ? "High" : "Low ";

		// TimeLib stores Year as number of years since 1970.
		Serial.print(String(te.tm.Year + 1970) + "-" + leadingDigit(te.tm.Month) + "-" + leadingDigit(te.tm.Day) + " ");
		Serial.print(leadingDigit(te.tm.Hour) + ":" + leadingDigit(te.tm.Minute) + " GMT");
		// Show tidal level to one decimal place
		Serial.println("  " + tideType + " tide of " + String(te.heightM, 1) + "m");
	}
}


void loop()
{
	// We're just doing a one off read of the API
	// so there's nothing to do here.
}
