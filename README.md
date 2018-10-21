# ESP8266 Admiralty Tiday API
Wrapper around the UK Admiralty's Tidal API.

This is currently a work in progress, but I'll update this when an example is added.

What's here right now works, but you'll need to do a little work yourself!

## Required Libraries

  * Json Streaming Parser
    
	See https://github.com/squix78/json-streaming-parser
	
  * ESP8266 WiFi
  
    See https://github.com/esp8266/Arduino
	
	If you get errors when the tidal client is trying to make a secure connection to the host then you probably need to update this project. There's a bug in earlier versions of the libaxtls.a library in the \tools\sdk\lib folder.
	
  * The Arduino Time Library
  
    See http://playground.arduino.cc/code/time