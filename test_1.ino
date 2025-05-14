#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOTtoken "YOUR_BOT_TOKEN_HERE"
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const int relayPin = D2;           // Output control (e.g., light)
const int ldrPin = A0;             // Light level sensor (LDR)
const int pirPin = D5;             // Motion sensor input (PIR)
const int sosLedPin = D6;          // LED for SOS Morse code

bool protection = true;
int chatID_acces[] = {}; // Add allowed chat IDs here

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

long Bot_lasttime;
const int Bot_mtbs = 3000;

String keyboardMain = "";
String keyboardStart = "";

void sendMorseSOS() {
  String morse = "... --- ..."; // SOS in Morse
  for (char c : morse) {
    if (c == '.') digitalWrite(sosLedPin, HIGH), delay(200);
    else if (c == '-') digitalWrite(sosLedPin, HIGH), delay(600);
    else delay(800); // space between letters
    digitalWrite(sosLedPin, LOW);
    delay(200);
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "") from_name = "Guest";

    bool authorized = !protection || (std::find(std::begin(chatID_acces), std::end(chatID_acces), chat_id.toInt()) != std::end(chatID_acces));

    if (!authorized) {
      if (text == "/start")
        bot.sendMessage(chat_id, "Access denied. Chat ID: " + chat_id, "");
      continue;
    }

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "This bot controls a smart light system via Telegram.\n\n";
      welcome += "/status - Light and motion info\n";
      welcome += "/switch - Toggle relay\n";
      welcome += "/sos - Blink SOS\n";
      bot.sendMessageWithInlineKeyboard(chat_id, welcome, "", keyboardStart);
    }

    if (text == "/switch") {
      digitalWrite(relayPin, !digitalRead(relayPin));
      bot.sendMessage(chat_id, String("Relay is now ") + (digitalRead(relayPin) ? "ON" : "OFF"), "");
    }

    if (text == "/status") {
      int light = analogRead(ldrPin);
      bool motion = digitalRead(pirPin);
      String message = "Light level: " + String(light) + "\n";
      message += "Motion detected: " + String(motion ? "Yes" : "No");
      bot.sendMessage(chat_id, message, "");
    }

    if (text == "/sos") {
      bot.sendMessage(chat_id, "Sending SOS signal...", "");
      sendMorseSOS();
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setInsecure();

  pinMode(relayPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(sosLedPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(sosLedPin, LOW);

  keyboardStart = "[[{ \"text\" : \"Status\", \"callback_data\" : \"/status\" }, { \"text\" : \"Switch\", \"callback_data\" : \"/switch\" }, { \"text\" : \"SOS\", \"callback_data\" : \"/sos\" }]]";
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs) {
    int newMsg = bot.getUpdates(bot.last_message_received + 1);
    while (newMsg) {
      handleNewMessages(newMsg);
      newMsg = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
