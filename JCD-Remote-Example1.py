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

print "Sending CMD_POWER_ON -> Projector turns on"
jcd.sendto("CMD_POWER_ON", (jcd_host, jcd_port))
time.sleep(10)
print "Sending CMD_TEST_PATTERN 7 -> Set testpattern to 7"
jcd.sendto("CMD_TEST_PATTERN 7", (jcd_host, jcd_port))
time.sleep(10)
print "Sending CMD_POWER_OFF-> Turn projector off"
jcd.sendto("CMD_TEST_PATTERN 7", (jcd_host, jcd_port))
print "End of demo"




