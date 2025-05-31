#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <base64.h>

#include "WebPage.h"

#define BUTTON 4
#define POWERSET 5
#define ISPCWORK 14

const char* ssid = "ollo";
const char* password = "123";

const char* www_username = "admin";
const char* www_password = "12341234123412341234";
const char* key = "12341234123412341234";

bool lastPCState = false;
bool pcState = false;
bool buttonState = false;
bool togglePC = false;

unsigned long enablePCMillis = 0;
const long enablePCInterval = 2000;

unsigned long wsCleanClientsMillis = 0;
const long wsCleanClientsInterval = 1000;

unsigned long checkButtonMillis = 0;
const long checkButtonInterval = 100;

unsigned long checkPCPowerMillis = 0;
const long checkPCPowerInterval = 50;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

char buffer[21];
String msg;
String encriptMsg;
String lastWsMessage;

bool isAuthenticated(AsyncWebServerRequest *request) {
  if (!request->authenticate(www_username, www_password)) {
    request->requestAuthentication();
    return false;
  }
  return true;
}

void notifyClients(const String &message) {
  if (ws.count() == 0) {
    Serial.println("[WebSocket] No clients connected");
    return;
  }
  ws.textAll(message);
  Serial.println("[WebSocket] " + message);
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, int len) {
  if (data == NULL) return;
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        if (len < sizeof(buffer)) 
        {
          memcpy(buffer, data, len);
          buffer[len] = '\0';
          msg = String(buffer);

          for(int i = 0; i < 20; i++)
            encriptMsg += (char(msg[i] ^ key[i]));
          Serial.println(encriptMsg);

          bool ans = false;
          if (encriptMsg[0] == 't' && encriptMsg[1] == 'o' && encriptMsg[2] == 'g') ans = true;
          if (encriptMsg[17] == 't' && encriptMsg[18] == 'o' && encriptMsg[19] == 'g') ans = true;
          if (encriptMsg[9] == 't' && encriptMsg[10] == 'o' && encriptMsg[11] == 'g') ans = true;

          if (ans)
          {
            togglePC = true;
          }

          encriptMsg = "";

          if (buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'a' && buffer[3] == 'd')  (pcState)? notifyClients("PC correctly work"): notifyClients("PC not work");

        } 
        else 
        {
          Serial.println("Too much data received!");
          return;
        }
        Serial.print("Received WS message: ");
        Serial.println(msg);
        }
      }
      break;
    case WS_EVT_ERROR:
      Serial.printf("WebSocket Error: %s\n", (char*)data);
      break;
    case WS_EVT_PONG:
      Serial.printf("WebSocket Pong\n");
      break;
  }
}



void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!isAuthenticated(request)) return;
    request->send_P(200, "text/html", htmlPage);
  });

  server.begin();
  Serial.println("HTTP server started");

  delay(100);

  pinMode(POWERSET, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(ISPCWORK, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - wsCleanClientsMillis >= wsCleanClientsInterval)
  {
    wsCleanClientsMillis = currentMillis;
    ws.cleanupClients(); 
  }


  if (currentMillis - enablePCMillis >= enablePCInterval)
  {
    enablePCMillis = currentMillis;
    
    togglePC = (buttonState)? true : togglePC;

    if (togglePC)
    {
      Serial.println("toggle PC");
        digitalWrite(POWERSET, HIGH);
        togglePC = false;
    }
    else
        digitalWrite(POWERSET, LOW);

  }

  if (currentMillis - checkButtonMillis >= checkButtonInterval)
  {
    checkButtonMillis = currentMillis;
    if (digitalRead(BUTTON) == LOW) buttonState = true;
    else buttonState = false;
  }

  if (currentMillis - checkPCPowerMillis >= checkPCPowerInterval)
  {
    checkPCPowerMillis = currentMillis;
    if (digitalRead(ISPCWORK) == HIGH) 
    {
      pcState = true;
      if(lastPCState != pcState)
        notifyClients("PC correctly work");
    }
    else 
    {
      pcState = false;
      if(lastPCState != pcState)
        notifyClients("PC not work");
    }
    lastPCState = pcState;
  }
}