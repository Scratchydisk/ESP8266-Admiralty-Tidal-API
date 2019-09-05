# ESP8266 Admiralty Tidal API

An ESP8266 / NodeMCU wrapper around the UK Admiralty's Tidal API.

This library provides a simple means to fetch a set of tidal event forecasts for a specific UK tidal station.

### To Do

* Create Arduino library Zip file (I use Visual Micro for Visual Studio so don't need this).

## API Definition

The [Admiralty Tidal API](https://admiraltyapi.portal.azure-api.net) is hosted and documented at https://admiraltyapi.portal.azure-api.net

You will need to register there for your free developer account and API key.

### Sample Code Fragments

**Obtain a set of tidal event forecasts for the next two days**

```C++    
WiFiClientSecure _wifiClient;
AdmiraltyApiClient _tidalApiClient;

// Initialise the API client's subscription key and station ID
_tidalApiClient.apiSubscriptionKey = "123456789abcdef";
_tidalApiClient.stationId = "0123";

// Fetch 2 days' forecasts
_tidalApiClient.FetchTidalEvents(_wifiClient, 2);

// Enumerate the results
for (int i = 0; i < _tidalApiClient.numberEvents; i++)
{
	TidalEvent te = _tidalApiClient.tidalEvents[i];

	// Do what you wish with the data...
	... te.tm.Day
 	... te.tm.Minute
    ... te.isHighTide
    ... te.heightM
    etc.
}
```
Sample Output
```
===== Fetched 8 Events =====
2018-10-28 01:53 GMT  High tide of 3.2m
2018-10-28 07:21 GMT  Low  tide of 0.5m
2018-10-28 14:01 GMT  High tide of 3.3m
2018-10-28 19:40 GMT  Low  tide of 0.5m
2018-10-29 02:34 GMT  High tide of 3.2m
2018-10-29 08:06 GMT  Low  tide of 0.6m
2018-10-29 14:40 GMT  High tide of 3.3m
2018-10-29 20:26 GMT  Low  tide of 0.6m
```
**Get the last and next tidal events (from current time) and display the hours and minutes from/to them**
```C++
TidalEvent teBefore = _tidalApiClient.PreviousTidalEvent(now());
TidalEvent teAfter = _tidalApiClient.NextTidalEvent(now());

if (teBefore.isValid)
{
	String tideType = teBefore.isHighTide ? "high" : "low";
	tmElements_t offset = teBefore.TimeFrom(now());
	Serial.println(String(offset.Hour) + ":" + String(offset.Minute) + " since last " + tideType + " tide");
}
if(teAfter.isValid)
{
	String tideType = teAfter.isHighTide ? "high" : "low";
	tmElements_t offset = teAfter.TimeFrom(now());
	Serial.println(String(offset.Hour) + ":" + String(offset.Minute) + " until next " + tideType + " tide");
}
```

Sample Output (relative to tidal events shown above)

```
20:39:44
0:59 since last low tide
5:54 until next high tide
```

### Tidal Station IDs

I haven't added support for the API calls that fetch the tidal station IDs simply because I only needed the one nearest my house!

Once you've registered for your developer account you'll have access to a ["try it" console](https://admiraltyapi.portal.azure-api.net/docs/services/uk-tidal-api/operations/Stations_GetStations/console) that you can use to find the station ID of interest to you.

## Example Sketch

The example sketch shows how to connect to the API and retrieve two days' tidal event forecasts and then output them in the format shown above via the serial port.

## Required Libraries

  * Json Streaming Parser
    
	See [https://github.com/squix78/json-streaming-parser](https://github.com/squix78/json-streaming-parser)
	
  * ESP8266 WiFi
  
    See [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)
	
>If you get errors when the tidal client is trying to make a secure connection to the host then you probably need to update this project. There's a bug in earlier versions of the libaxtls.a library in the \tools\sdk\lib folder.
>
>Code changed to work with BearSSL so this shouldn't be an issue anymore.
	
  * The Arduino Time Library
  
    See [http://playground.arduino.cc/code/time](http://playground.arduino.cc/code/time)

## Coding Style

I've always been a fan of clarity over obfuscation and this is reflected in the way I use brackets in my code. Please don't post comments on this as I don't want to engage in a style war!

I strongly recommend the [Barr Group's Embedded C Coding Standard](https://barrgroup.com/Embedded-Systems/Books/Embedded-C-Coding-Standard).

## Change Log
* 23 Aug 2019 - ESP8266 library uses BearSSL rather than axTLS now.
    * Minor change needed to support non validation of TLS certificates. 

* 28 Oct 2018 - Added time related functionality:
    * TidalEvent structure changed to class
	* TidalEvent epoch (UNIX) time and isValid attributes
	* TidalEvent TimeFrom method to get time period between event and supplied time value
	* PreviousTidalEvent method
	* NextTidalEvent method
	

* 23 Oct 2018 - Initial release with example

> Written with [StackEdit](https://stackedit.io/).