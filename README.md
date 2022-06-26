# Lighweight Cryptography Project
Code for my Introduction to Cryptography course project
Using an Arduino (atmega328p) and Raspberry PI 3 I explore two of the finalist algorithms for NIST's Lightweight Cryptography competition: Sparkle and Xoodyak.

# Setup and Run
Right now, the code is messy.
My current goal is to provide a way to correctly run the algorithms and test on some IoT scenarios:
* Send a RFID card number
* Send readings from distance sensor (HC-04)
* Receive light ON/OFF commands

That said, Makefile has rules for creating test and arduino binaries.
> Makefile needs some work to avoid duplicating so much the code

Two links are established with arduino: one through bluetooth, other through usb.

Usb is used to send commands and change opperation mode (idle, read distance Sensor, read tag, receive light command), and also debug.
Bluetooth is the data path: encrypted data is sent and received here.

Once you have build the programs to encrypt an decrypt, and arduino is running with bluetooth connected, to receive and send data run:
python tty2print.py --encrypt ./bin/encrypt_sparkle --decrypt ./bin/decrypt_sparkle
