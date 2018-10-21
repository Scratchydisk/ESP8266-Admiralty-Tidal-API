/*
 Name:		AdmiraltyTidalApiClient.h
 Created:	10/21/2018 10:43:30 AM
 Author:	Stewart McSporran
 Editor:	http://www.visualmicro.com

 Released under MIT license.
*/

#ifndef _AdmiraltyTidalApiClient_h
#define _AdmiraltyTidalApiClient_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

// Enable this define to see debug messages when using the API
#define DEBUG_ADMIRALTY

#ifdef DEBUG_ADMIRALTY
#define DEBUGV(...) Serial.println(__VA_ARGS__)
#else
#define DEBUGV(...)
#endif

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <TimeLib.h>


/*******************************/

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

// *.azure-api.net
// Expires 11 Jan 2020
#define SSL_THUMBRPRINT_API "cb33e927a76acb77c8215aaf4f98a3ff78139069"  

/**** ADMIRALTY TIDAL API URLs *********/
const String ADMIRALTY_API_HOST = "admiraltyapi.azure-api.net";

// Need to add StationId as a suffix to this call
const String ADMIRALTY_STATIONS_URL = "https://" + ADMIRALTY_API_HOST + "/uktidalapi/api/V1/Stations/";

// Stores the details of each tidal event
struct TidalEvent
{
	bool isHighTide;
	// ISO 8601 time stamp from API in GMT
	String dateTime;
	// Parsed from the dateTime string (in GMT)
	tmElements_t tm;
	// Height in metres
	float heightM;
};


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
	// Attempt to validate the SSL certificate thumbprint? (Default is false)
	bool validateSslThumbprint;

	// Auth key for the Admiralty API
	String apiSubscriptionKey;

	// Identifier of the tidal station
	String stationId;

	// Number of tidal events parsed
	uint8_t numberEvents;

	// Collection of parsed tidal events
	TidalEvent tidalEvents[MAX_COUNT_TIDAL_EVENTS];

	// ctor
	AdmiraltyApiClient();

	// Read tidal events from the Admiralty API for the supplied station and number of days forecast.
	// Returns an ADMIRALTY_API status code.
	uint8_t FetchTidalEvents(WiFiClientSecure wifiClient, uint8_t numberDays);

};

#endif


