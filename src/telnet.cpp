#include "telnet.h"
#include <ESP8266WiFi.h>
#include <adri_tools_v2_serialMenu.h>

WiFiServer adri_telenet_server(23);
WiFiClient adri_telenet_client;

adri_telnet::adri_telnet(){

}
void adri_telnet::telnet_setup() {
      adri_telenet_server.begin();
      adri_telenet_server.setNoDelay(true);
      Serial.println("Telnet Client can connect");
}
void adri_telnet::telnet_loop() {
    // look for Client connect trial
    if (adri_telenet_server.hasClient()) {
        if (!adri_telenet_client || !adri_telenet_client.connected()) {
            if (adri_telenet_client) {
                adri_telenet_client.stop();
                Serial.println("Telnet Client Stop");
            }
            adri_telenet_client = adri_telenet_server.available();
            Serial.println("New Telnet client");
            adri_telenet_client.flush();  // clear input buffer, else you get strange characters
        }
    }
    String input = "";
    if (telnet_get(input)) {
        // Serial.println(input);
        if(input != "") adriToolsv2_serialReadPtr_get()->read(input);
    }    
}

// when we want to send data to telnet
// void adri_telnet::telnet_put(char * output_string) {
//     if (adri_telenet_client && adri_telenet_client.connected()) {
//         adri_telenet_client.print(output_string);
//     }
// }

void adri_telnet::telnet_put(String output_string) {
    if (adri_telenet_client && adri_telenet_client.connected()) {
        adri_telenet_client.print(output_string.c_str());
    }
}

// when we want to get data from telnet
#define SOP '<'
#define EOP '>'
bool started = false;
bool ended = false;
char inData[16];//13 for vars + SOP +EOP +NULL = 16 total (if this is set for < 16 < you may see chars cut-off/missing or connections repudiated
byte indexA;
boolean adri_telnet::telnet_get(String & input_string) {
    if (adri_telenet_client.available()) {
        input_string = "";
        while (adri_telenet_client.available())
        {
          char inChar = adri_telenet_client.read();
          if (inChar == SOP)
          {
            indexA = 0;
            inData[indexA] = '\0';
            started = true;
            ended = false;
          }
          else if (inChar == EOP)
          {
            ended = true;
            break;
          }
          else
          {
            //if this is less than your char inData setting then you'll see that characters are missing, <16 in this case
            //NOTE : this can be larger that your char inData setting from what I've experienced in testing
            if (indexA < 16)
            {

              inData[indexA] = inChar;
              indexA++;
              inData[indexA] = '\0';
            }
          }

        }
        if (started && ended)
        {
          // The end of packet marker arrived. Process the packet
          input_string = String(inData);  
          // fsprintf("\n[telenet get] : %s\n", input_string.c_str());
          //sscanf(inData, "%s,%s,%s,%s", A, B, C, D); //with this enabled the ESP crashed and may even reboot , thereafter refusing connections
          started = false;
          ended = false;
          indexA = 0;
          inData[indexA] = '\0';
        }
        return true;
    } else {
        return false;
    }
     return false;
}