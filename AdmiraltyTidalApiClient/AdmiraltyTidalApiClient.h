/*
 Name:		AdmiraltyTidalApiClient.h
 Created:	10/21/2018 10:43:30 AM
 Author:	Stewart McSporran
 Editor:	http://www.visualmicro.com

 A library that acts as a client to the UK Admiralty's
 Tidal forecaset API.

 See https://admiraltyapi.portal.azure-api.net

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

#ifndef _AdmiraltyTidalApiClient_h
#define _AdmiraltyTidalApiClient_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

// Enable this define to see debug messages when using the API
//#define DEBUG_ADMIRALTY

#ifdef DEBUG_ADMIRALTY
#define DEBUGV(...) Serial.println(__VA_ARGS__)
#else
#define DEBUGV(...)
#endif

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <ezTime.h>


/*******************************/

// Maximum number of predections to parse
#define MAX_COUNT_TIDAL_EVENTS 30

/**** Status codes *******/
#define ADMIRALTY_API_SUCCESS 0
#define ADMIRALTY_API_HOSTBYNAME_FAILED 1
#define ADMIRALTY_API_CONNECT_FAILED 2
#define ADMIRALTY_API_SSL_VALIDATION_FAILED 3

/***** Thumbprint of the API's certificate ****
* If you're using this then you'll need to update it
* when the certificate is renewed.
*
* If you're really worried about security then you
* should manually check the API URL and certificate
* thumbprint against the API website to make sure
* this file hasn't been changed be some third party!
****/

// API uses an Azure wildcard certificate
// Subject: *.azure-api.net
// Expires: 11 Jan 2020
#define SSL_THUMBRPRINT_API "cb33e927a76acb77c8215aaf4f98a3ff78139069"  

// Host name of the tidal API
const String ADMIRALTY_API_HOST = "admiraltyapi.azure-api.net";

// Base API URL to fetch tidal station data.
const String ADMIRALTY_STATIONS_URL = "https://" + ADMIRALTY_API_HOST + "/uktidalapi/api/V1/Stations/";

// Class to store the details of each tidal event
class TidalEvent
{
public:
	// True if the record is a valid forecast.
	// False typically implies a search method couldn't find a matching entry in the cache.
	// i.e. trying to find the event after a time and the last entry is before the time.
	bool isValid;
	// True if the tidal forecast is for a high tide
	bool isHighTide;
	// ISO 8601 time stamp from API in GMT
	String dateTime;
	// Parsed time structure from the dateTime string (in GMT)
	tmElements_t tm;
	// Epoch time representation
	time_t epochTime;
	// Height in metres
	float heightM;

	// Number of hours and minutes between this event and the time parameter.
	tmElements_t TimeFrom(time_t time);
};


// Client for the UK Admiralty's Tidal API.
// Only the tidal events (forecasts) are supported.
// The tidal station details and lists have not been
// added as I just don't need them right now.
class AdmiraltyApiClient : public JsonListener
{
private:
	uint8_t eventIndex;
	String currentKey;
	JsonStreamingParser jsonParser;
	void convertFromIso8601(String time_string, tmElements_t &tm_data);

protected:
	// Json parser overrides...
	virtual void whitespace(char c);
	virtual void startDocument();
	virtual void key(String key);
	virtual void value(String value);
	virtual void endArray();
	virtual void endObject();
	virtual void endDocument();
	virtual void startArray();
	virtual void startObject();

public:
	// If true the connection will attempt to validate the SSL 
	// certificate against the API certificate's thumbprint. (Default is false)
	bool validateSslThumbprint;

	// Auth key for the Admiralty API
	String apiSubscriptionKey;

	// Identifier of the tidal station
	String stationId;

	// Number of tidal events stored in the tidalEvents attribute (read only)
	uint8_t numberEvents;

	// Collection of parsed tidal events
	TidalEvent tidalEvents[MAX_COUNT_TIDAL_EVENTS];

	// ctor
	// Defaults the certificate validation to false
	AdmiraltyApiClient();

	// Read tidal events from the Admiralty API for the supplied station and number of days forecast.
	// Max value for numberDays is 7.
	// Returns an ADMIRALTY_API_XXX status code.
	uint8_t FetchTidalEvents(WiFiClientSecure wifiClient, uint8_t numberDays);

	// Finds the tidal event immediately prior to the specified time
	TidalEvent PreviousTidalEvent(time_t time);

	// Finds the tidal event immediately after to the specified time
	TidalEvent NextTidalEvent(time_t time);
};

#endif


