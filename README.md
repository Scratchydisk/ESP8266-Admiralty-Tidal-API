# ESP8266 Admiralty Tidal API

An ESP8266 / NodeMCU wrapper around the UK Admiralty's Tidal API.

This library provides a simple means to fetch a set of tidal event forecasts for a specific UK tidal station.

### To Do

* Create Arduino library Zip file (I use Visual Micro for Visual Studio so don't need this).

## API Definition

The [Admiralty Tidal API](https://admiraltyapi.portal.azure-api.net) is hosted and documented at https://admiraltyapi.portal.azure-api.net

You will need to register there for your free developer account and API key.

### Simplified Overview

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

### Tidal Station IDs

I haven't added support for the API calls that fetch the tidal station IDs simply because I only needed the one nearest my house!

Once you've registered for your developer account you'll have access to a ["try it" console](https://admiraltyapi.portal.azure-api.net/docs/services/uk-tidal-api/operations/Stations_GetStations/console) that you can use to find the station ID of interest to you.

## Example Sketch

The example sketch shows how to connect to the API and retrieve two days' tidal event forecasts and then output them in the following format via the serial port.

    ===== Fetched 8 Events =====
    2018-10-23 04:28 GMT  Low  tide of 0.5m
    2018-10-23 11:10 GMT  High tide of 3.0m
    2018-10-23 16:37 GMT  Low  tide of 0.8m
    2018-10-23 23:24 GMT  High tide of 3.1m
    2018-10-24 05:02 GMT  Low  tide of 0.4m
    2018-10-24 11:43 GMT  High tide of 3.0m
    2018-10-24 17:08 GMT  Low  tide of 0.7m
    2018-10-24 23:58 GMT  High tide of 3.1m

## Required Libraries

  * Json Streaming Parser
    
	See [https://github.com/squix78/json-streaming-parser](https://github.com/squix78/json-streaming-parser)
	
  * ESP8266 WiFi
  
    See [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)
	
>If you get errors when the tidal client is trying to make a secure connection to the host then you probably need to update this project. There's a bug in earlier versions of the libaxtls.a library in the \tools\sdk\lib folder.
	
  * The Arduino Time Library
  
    See [http://playground.arduino.cc/code/time](http://playground.arduino.cc/code/time)

## Coding Style

I've always been a fan of clarity over obfuscation and this is reflected in the way I use brackets in my code. Please don't post comments on this as I don't want to engage in a style war!

I strongly recommend the [Barr Group's Embedded C Coding Standard](https://barrgroup.com/Embedded-Systems/Books/Embedded-C-Coding-Standard).

## Change Log
* 23 Oct 2018 - Initial release with example
