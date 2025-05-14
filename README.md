# Telegrambot_Home_automation
# ESP8266 Telegram Bot – Smart Light Control System

This project is a **template-based smart control system** using an ESP8266 microcontroller. It connects to a Telegram bot, allowing users to remotely control devices, monitor environmental data (light level and motion), and send an emergency **Morse code SOS signal** using a simple LED.

This sketch can serve as a foundation for more advanced IoT and automation systems.

---

## Features

- Control devices (e.g., lamps) remotely via Telegram
- Monitor ambient **light levels** using an LDR sensor
- Detect **motion** using a PIR sensor
- Trigger a **Morse code SOS signal** via LED
- Modular design for easy scaling and individual customization

---

##  Required Hardware

| Component        | Quantity | Notes                                    |
|------------------|----------|------------------------------------------|
| ESP8266 board    | 1        | NodeMCU or WeMos D1 Mini recommended     |
| Relay module     | 1        | To control lamps or other 220V devices   |
| LDR (Light sensor)| 1       | Connect to analog input (A0)             |
| 10kΩ resistor     | 1        | For voltage divider with LDR            |
| PIR Motion Sensor| 1        | e.g., HC-SR501                           |
| LED              | 1        | Used for Morse code signaling            |
| Resistor (220Ω)  | 1        | For the LED                              |
| Jumper wires     | ~10      | For connections                          |
| Breadboard       | Optional | For prototyping                          |

---

##  Telegram Bot Setup

1. Search for `@BotFather` in Telegram.
2. Create a new bot with `/newbot` and follow the instructions.
3. Copy the bot token and replace it in your code:
   ```cpp
   #define BOTtoken "YOUR_BOT_TOKEN"
