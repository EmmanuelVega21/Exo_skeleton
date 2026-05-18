#!/usr/bin/env python3
# Raspberry Pi Code - Exoskeleton Control
# This code runs on the Raspberry Pi

import serial
import time

PORT = "/dev/ttyACM0"  # change if needed
BAUD = 115200          # <<< MATCHES Serial.begin(115200) on Arduino

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # give Arduino time to reset

print("Listening for FSR data on", PORT)

try:
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line:
            print("RAW:", line)   # temporarily print everything
            if line.startswith("FSR:"):
                try:
                    value = int(line.split(":")[1])
                    print("FSR raw:", value)
                except ValueError:
                    pass
except KeyboardInterrupt:
    print("\nStopping...")
finally:
    ser.close()
