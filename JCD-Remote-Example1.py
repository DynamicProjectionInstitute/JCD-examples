#!/usr/bin/env python3
# Sample for remote control of the JCD
#
# Copyright (c) 2017 by Dynamic Projeciton Institute GmbH, Vienna, Austria
# Author: Martin Willner <willner@dynamicprojection.com>


import socket
import time


jcd_host="2.0.0.20"
jcd_port=7781


jcd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# For more commands please see the JCD manual.

print("Sending ON -> Projector turns on")
jcd.sendto("ON".encode(), (jcd_host, jcd_port))
time.sleep(10)
print("Sending PATTERN 7 -> Set testpattern to 7")
jcd.sendto("TEST 7".encode(), (jcd_host, jcd_port))
time.sleep(10)
print("Sending OFF-> Turn projector off")
jcd.sendto("OFF".encode(), (jcd_host, jcd_port))
print("End of demo")




