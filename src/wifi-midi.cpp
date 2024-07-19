#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <AppleMIDI.h>
#include <Control_Surface.h>
#include <MIDI_Interfaces/Wrappers/FortySevenEffects.hpp>

#define SerialMon Serial
#define USE_EXT_CALLBACKS

int8_t isConnected = 0;

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "GuitarMod", DEFAULT_CONTROL_PORT);
FortySevenEffectsMIDI_Interface<decltype(MIDI) &> AppleMIDI_interface = MIDI;

CCPotentiometer potentiometer{
    3,
    {MIDI_CC::General_Purpose_Controller_1},
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200); // STart serial for monitoring/debugging

  WiFiManager wm;

  bool res;
  wm.setHostname("MIDIGuitarKnob");
  res = wm.autoConnect();

  if (!res)
    Serial.println("Failed to connect to WiFi!");

  else
  {

    Serial.println("Connected to network!");
    Serial.println("OK, now make sure you an rtpMIDI session that is Enabled");
    Serial.println("Add device named Arduino with Host");
    Serial.println(String(WiFi.localIP()));
    Serial.println(WiFi.macAddress());

    if (!MDNS.begin(AppleMIDI.getName()))
      Serial.println("Error setting up MDNS responder");

    AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t &ssrc, const char *name)
                                 {
        isConnected++;
        Serial.println("Connected to session" + String(name)); 
        Serial.println(String(ssrc)); });
    AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t &ssrc)
                                    {
        isConnected--;
        Serial.println("Disconnected");
        Serial.println(String(ssrc)); });

    MDNS.addService("apple-midi", "udp", AppleMIDI.getPort());

    Control_Surface.begin();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  Control_Surface.loop();
}
