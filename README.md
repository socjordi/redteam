# Red Team Exercises - Physical Attacks

These exercises focused on evaluating physical security. The primary objective of the exercises was to identify vulnerabilities in physical security measures, operational procedures, and access controls by simulating the methods used by malicious actors.
These exercises not only aim to identify weaknesses but also provide practical recommendations to strengthen security and improve response capabilities against potential physical threats.

The goal was to find small, easily hidden, and as cheap as possible materials.

## EXERCISE 1. The physical access

Once it was discovered that physical access was controlled using MIFARE Classic contactless smart cards and that the control was based on the card's identifier, a device was set up to capture card identifiers. The device could be placed next to the legitimate reader and remain unnoticed. It stores the identifiers in Flash memory, and the collector retrieves the list via Wi-Fi.

<img src="https://github.com/socjordi/redteam/blob/main/images/IMG_20250301_180230.jpg" width="800"/>
<img src="https://github.com/socjordi/redteam/blob/main/images/IMG_20250301_175944.jpg" width="800"/>
<img src="https://github.com/socjordi/redteam/blob/main/images/card_reader.gif" width="800"/>

Bill of materials:
- Module ESP32-S3-DevKitC-1-N16R8V (10.50 €)
- Module RFID RC522 (1.25 €)
- USB Rechargeable Battery 10000 mAh (4.00 €)

## EXERCISE 2. The infrastructure

An infrastructure was set up that the attacker could quickly deploy once inside the facility. This infrastructure would allow them to exfiltrate information to internet without being detected by intrusion detection systems, carry out remote attacks, capture network traffic, record conversations, and capture passwords or any text typed on a USB keyboard.

<img src="https://github.com/socjordi/redteam/blob/main/images/Esquema.png" width="800"/>

### The access to Internet

One of the cheapest 4G routers on the market was chosen. It will create a local Wi-Fi network with internet access for the rest of the infrastructure (the collector, the keylogger, the access card capturer, etc.).

<img src="https://github.com/socjordi/redteam/blob/main/images/IMG_20250301_181940.jpg" width="800"/>
<img src="https://github.com/socjordi/redteam/blob/main/images/4g.gif" width="800"/>

Bill of materials:
- Router 4G LDW931 (12.00 €)

### The Collector

It is a mini-computer that collects keylogger captures, network traffic, access card identifiers, conversation recordings, etc., and sends them to a C2 server on the internet, which is accessible to the attacker. By means of a LAN USB adapter, a second network interface was created to do remote attacks.

<img src="https://github.com/socjordi/redteam/blob/main/images/IMG_20250301_182440.jpg" width="800"/>
<img src="https://github.com/socjordi/redteam/blob/main/images/collector.gif" width="800"/>

Bill of materials:
- Raspberry Pi 4 Model B (1 GB, 52.50 €)
- LAN USB adapter (9.00 €)
- Power Supply USB-C 3A (8.50 €)
- Microphone USB (2.00 €)

### The Network Sniffer

A small (100x23x64mm) and inexpensive 5 Gigabit ports ethernet switch model was chosen, but one that had the capability to perform port mirroring. The main objective was to capture unencrypted network traffic. The switch was configured so that all inbound and outbound network traffic in ports 1- 4 was copied to port 5, which was sniffed by the collector. A second network interface in the collector was connected to one of ports 1-4 to do remote attacks.

<img src="https://github.com/socjordi/redteam/blob/main/images/Switch.png" width="800"/>

Bill of materials:
- Ethernet Switch Tenda TEG205E (14.60 €)

### The Keylogger

Even though it is not the cheapest, the smallest keylogger on the market with built-in Wi-Fi access was chosen. It’s body is only 15x10x10mm.

<img src="https://github.com/socjordi/redteam/blob/main/images/Keylogger.png" width="800"/>
<img src="https://github.com/socjordi/redteam/blob/main/images/keylogger.gif" width="800"/>

Bill of materials:
- KeyDemon KeyGrabber Air USB (60.00 €)
