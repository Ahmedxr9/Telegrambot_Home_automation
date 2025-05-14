#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// === الإعدادات الأساسية ===
#define BOTtoken "8152499702:AAEZ5yaJ9KF3b6cSMezcX0nb0yacTzCQUKQ"
char ssid[] = "Convex_AANH-5G";
char password[] = "42818618";
String buttons[] = {"Light"};
int pin[] = {2};

bool protection = 1;
int chatID_acces[] = {};

String on_symbol = "✅ ";
String off_symbol = "☑ ";

// === المستشعرات والمكونات الإضافية ===
#define ldrPin A0
#define pirPin D5
#define sosLedPin D6

// === كائنات الاتصال ===
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// === متغيرات تشغيل البوت ===
int quantity;
int Bot_mtbs = 3000;
long Bot_lasttime;
String keyboardJson = "";
String keyboardStart = "";

// === دالة إرسال كود SOS ===
void sendMorseSOS() {
  String morse = "... --- ...";
  for (char c : morse) {
    if (c == '.') {
      digitalWrite(sosLedPin, HIGH); delay(200);
    } else if (c == '-') {
      digitalWrite(sosLedPin, HIGH); delay(600);
    } else {
      delay(800);
    }
    digitalWrite(sosLedPin, LOW);
    delay(200);
  }
}

// === دالة التعامل مع الرسائل الجديدة ===
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String m_id = String(bot.messages[i].message_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (bot.messages[i].type == "callback_query") {
      String statusMessage;
      for (int i1 = 0; i1 < quantity; i1++) {
        if (text == buttons[i1]) {
          digitalWrite(pin[i1], !digitalRead(pin[i1]));
        }
        digitalRead(pin[i1]) ? statusMessage += on_symbol : statusMessage += off_symbol;
        statusMessage += buttons[i1];
        statusMessage += '\n';
      }
      bot.sendMessageWithInlineKeyboard(chat_id, statusMessage, "", keyboardJson);
    } else {
      int i2 = 0;
      do {
        if (!protection || String(chatID_acces[i2]) == chat_id) {
          if (text == "/switch") {
            String statusMessage;
            for (int i = 0; i < quantity; i++) {
              digitalRead(pin[i]) ? statusMessage += on_symbol : statusMessage += off_symbol;
              statusMessage += buttons[i];
              statusMessage += '\n';
            }
            bot.sendMessageWithInlineKeyboard(chat_id, statusMessage, "", keyboardJson);
          }

          if (text == "/status") {
            int lightLevel = analogRead(ldrPin);
            bool motionDetected = digitalRead(pirPin);
            String status = "🌞 Light level: " + String(lightLevel) + "\n";
            status += motionDetected ? "🚶 Motion detected\n" : "🛑 No motion\n";
            bot.sendMessage(chat_id, status, "");
          }

          if (text == "/sos") {
            bot.sendMessage(chat_id, "🚨 Sending SOS signal...", "");
            sendMorseSOS();
          }

          if (text == "/start") {
            String welcome = "Welcome " + from_name + ".\n";
            welcome += "This is ESP8266 smart switch, controlled via Telegram.\n\n";
            welcome += "/switch : Click to open Switch Board.\n";
            welcome += "/status : Show light & motion status.\n";
            welcome += "/sos : Send SOS signal via LED.\n";
            bot.sendMessageWithInlineKeyboard(chat_id, welcome, "", keyboardStart);
          }
          break;
        } else {
          if ((i2 == ((sizeof(chatID_acces) / sizeof(int)) - 1) && text == "/start") ||
              ((sizeof(chatID_acces) / sizeof(int)) == 0 && text == "/start")) {
            bot.sendMessage(chat_id, "No access, Chat ID: " + chat_id, "");
          }
        }
        i2++;
      } while (i2 < (sizeof(chatID_acces) / sizeof(int)));
    }
  }
}

// === دالة الإعداد ===
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  quantity = sizeof(pin) / sizeof(int);
  for (int i = 0; i < quantity; i++) {
    pinMode(pin[i], OUTPUT);
  }

  pinMode(pirPin, INPUT);
  pinMode(sosLedPin, OUTPUT);

  for (int i = 0; i < quantity; i++) {
    if (i == 0) keyboardJson += "[";
    keyboardJson += "[{ \"text\" : \"" + buttons[i] + "\", \"callback_data\" : \"" + buttons[i] + "\" }]";
    if (i == quantity - 1) {
      keyboardJson += "]";
    } else {
      keyboardJson += ",";
    }
  }

  // إعداد لوحة المفاتيح الخاصة بـ /start
  keyboardStart = "[[{ \"text\" : \"Switch Board\", \"callback_data\" : \"/switch\" }]]";

  delay(10);
  client.setInsecure();
}

// === الحلقة الرئيسية ===
void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
