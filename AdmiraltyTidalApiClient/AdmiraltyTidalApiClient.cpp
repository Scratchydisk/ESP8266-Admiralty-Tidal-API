/*
 Name:		AdmiraltyTidalApiClient.cpp
 Created:	10/21/2018 10:43:30 AM
 Author:	Stewart
 Editor:	http://www.visualmicro.com
*/

#include "AdmiraltyTidalApiClient.h"

// Default ctor
// Sets the SSL validation option to none.
AdmiraltyApiClient::AdmiraltyApiClient()
{
	jsonParser.setListener(this);
	validateSslThumbprint = false;
}

// Read tidal events from the Admiralty API for the supplied station and number of days forecast.
uint8_t AdmiraltyApiClient::FetchTidalEvents(WiFiClientSecure wifiClient, uint8_t numberDays)
{
	// Can we resolve the host IP?
	IPAddress hostIp;
	if (!WiFi.hostByName(ADMIRALTY_API_HOST.c_str(), hostIp))
	{
		DEBUGV("HostByName failed for " + String(ADMIRALTY_API_HOST));
		return ADMIRALTY_API_HOSTBYNAME_FAILED;
	}

	DEBUGV("IP of " + ADMIRALTY_API_HOST + " is " + hostIp.toString());
	DEBUGV("\nStarting connection to server...");

	if (!wifiClient.connect(ADMIRALTY_API_HOST, 443))
	{
		DEBUGV("Connection failed!");
		return ADMIRALTY_API_CONNECT_FAILED;
	}

	if (validateSslThumbprint)
	{
		bool sslValid = wifiClient.verify(SSL_THUMBRPRINT_API, ADMIRALTY_API_HOST.c_str());
		if (!sslValid)
		{
			DEBUGV("SSL Validation failed");
			return ADMIRALTY_API_SSL_VALIDATION_FAILED;
		}
	}

	DEBUGV("Connected to server.  Calling...");
	//String fullGet = "GET " + ADMIRALTY_STATIONS_URL + AYR_STATION_ID + " HTTP/1.1";
	String fullGet = "GET " + ADMIRALTY_STATIONS_URL + stationId + "/TidalEvents?duration=" + String(numberDays) + " HTTP/1.1";

	DEBUGV(fullGet);

	// Make the HTTP request:
	wifiClient.println(fullGet);
	wifiClient.println("Host: " + ADMIRALTY_API_HOST);
	wifiClient.println("Ocp-Apim-Subscription-Key:" + apiSubscriptionKey);
	wifiClient.println("Connection: close");
	wifiClient.println();

	// Read through the headers...
	// Two \r in a row indicate headers complete
	while (wifiClient.connected())
	{
		String line = wifiClient.readStringUntil('\n');
		if (line == "\r")
		{
			DEBUGV("Finished Receiving Headers");
			break;
		}
		DEBUGV(line);
	}

	// Read and parse the body of the message
	while (wifiClient.available())
	{
		char c = wifiClient.read();
		jsonParser.parse(c);
	}

	wifiClient.stop();

	return ADMIRALTY_API_SUCCESS;
}


void AdmiraltyApiClient::whitespace(char c)
{
	DEBUGV("whitespace");
}

void AdmiraltyApiClient::startDocument() {
	DEBUGV("start document");
	eventIndex = 0;
	numberEvents = 0;
}

void AdmiraltyApiClient::key(String key) {
	DEBUGV("key: " + key);
	currentKey = key;
}

void AdmiraltyApiClient::value(String value) {
	DEBUGV("value: " + value);
	if (currentKey == "EventType")
	{
		tidalEvents[eventIndex].isHighTide = value == "HighWater";
	}
	else if (currentKey == "DateTime")
	{
		tidalEvents[eventIndex].dateTime = value;
		memset(&(tidalEvents[eventIndex].tm), 0, sizeof(tmElements_t));
		convertFromIso8601(value, tidalEvents[eventIndex].tm);
	}
	else if (currentKey == "Height")
	{
		tidalEvents[eventIndex].heightM = value.toFloat();
	}
}

void AdmiraltyApiClient::endArray() {
	DEBUGV("end array. ");
}

void AdmiraltyApiClient::endObject() {
	DEBUGV("end object. ");
	if (eventIndex < (MAX_COUNT_TIDAL_EVENTS - 1))
	{
		eventIndex++;
	}
}

void AdmiraltyApiClient::endDocument() {
	DEBUGV("end document. ");
	numberEvents = eventIndex;
}

void AdmiraltyApiClient::startArray() {
	DEBUGV("start array. ");
}

void AdmiraltyApiClient::startObject() {
	DEBUGV("start object. ");
}

void AdmiraltyApiClient::convertFromIso8601(String time_string, tmElements_t & tm_data)
{
	// 2018-10-17T17:25:00
	// Y - 0 4
	// M - 5 2
	// D - 8 2
	// H - 11 2
	// MM - 14 2
	// S - 17 2
	// Can be fractional seconds but we'll ignore them

	DEBUGV("Year: " + time_string.substring(0, 4));
	DEBUGV("Month: " + time_string.substring(5, 7));
	DEBUGV("Day: " + time_string.substring(8, 10));

	tm_data.Year = time_string.substring(0, 4).toInt() - 1970;
	tm_data.Month = time_string.substring(5, 7).toInt();
	tm_data.Day = time_string.substring(8, 10).toInt();
	tm_data.Hour = time_string.substring(11, 13).toInt();
	tm_data.Minute = time_string.substring(14, 16).toInt();
	tm_data.Second = time_string.substring(17, 19).toInt();
}