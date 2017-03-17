#!/usr/bin/env python
from bluepy.btle import UUID, Peripheral, DefaultDelegate
import struct
import math
from collections import deque
import sys
#import serial, sys, time, string, pygame

G=9.8

def _TI_UUID(val):
    return UUID("%08X-0451-4000-b000-000000000000" % (0xF0000000+val))

class SensorBase:
    # Derived classes should set: svcUUID, ctrlUUID, dataUUID
    sensorOn  = struct.pack("B", 0x01)
    sensorOff = struct.pack("B", 0x00)

    def __init__(self, periph):
        self.periph = periph
        self.service = None
        self.ctrl = None
        self.data = None

    def enable(self):
        if self.service is None:
            self.service = self.periph.getServiceByUUID(self.svcUUID)
        if self.ctrl is None:
            self.ctrl = self.service.getCharacteristics(self.ctrlUUID) [0]
        if self.data is None:
            self.data = self.service.getCharacteristics(self.dataUUID) [0]
        if self.sensorOn is not None:
            self.ctrl.write(self.sensorOn,withResponse=True)

    def read(self):
        return self.data.read()

    def disable(self):
        if self.ctrl is not None:
            self.ctrl.write(self.sensorOff)

    # Derived class should implement _formatData()

class SmartTag(Peripheral):
    def __init__(self, addr, addrType="random"):
        Peripheral.__init__(self,addr, addrType="random")
        self.discoverServices()
        #self.movement = MovementSensor(self)
        self.hx_scale = HxScale(self)
        print "geting services"

class HxScale(SensorBase):
    def __init__(self, periph):
        SensorBase.__init__(self, periph)

        self.svcUUID  = UUID(0xFA00)
        self.ctrlUUID = UUID(0xFA01)
        self.dataUUID = UUID(0xFA02)
    def enable(self):
        if self.service is None:
            self.service = self.periph.getServiceByUUID(self.svcUUID)
        if self.ctrl is None:
            self.ctrl = self.service.getCharacteristics(self.ctrlUUID) [0]
        if self.data is None:
            self.data = self.service.getCharacteristics(self.dataUUID) [0]

    def notify(self):

        #self.periph.writeCharacteristic(0x17, struct.pack('<bb', 0x01, 0x00))
        self.char_descr = self.service.getDescriptors(forUUID=0x2902)[0]
        self.char_descr.write(struct.pack('<bb', 0x01, 0x00), True)


    def write(self, cmd):
        self.ctrl.write(cmd, withResponse=True)

SK=(1.49/G, 0.71/G, 0.73/G, 0.71/G)
class HxScaleDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
        self.scale_deque = deque()
	#self.fd_scale = open('scale.dat', 'w')

    def handleNotification(self, hnd, data):
        data_ex =  struct.unpack('iiiii', data)
        #print "scale:",":".join("{:02x}".format(c) for c in data_ex) 
        print data_ex[1]*SK[0], data_ex[2]*SK[1], data_ex[3]*SK[2], data_ex[4]*SK[3]
        #print data_ex[0],data_ex[1], data_ex[2], data_ex[3]
	#newFileByteArray = bytearray(data[4:])
        #self.sync_deque.append(data)

    def get_deque(self):
        return self.scale_deque

if __name__ == "__main__":
    import time
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('host', action='store',help='MAC of BT device')
    parser.add_argument('-n', action='store', dest='count', default=0,
            type=int, help="Number of times to loop data")
    parser.add_argument('-t',action='store',type=float, default=5.0, help='time between polling')
    parser.add_argument('-H','--hx', action='store_true', default=False)
    parser.add_argument('--all', action='store_true', default=False)

    arg = parser.parse_args(sys.argv[1:])

    print('Connecting to ' + arg.host)
    tag = SmartTag(arg.host, addrType="random")

    # Enabling selected sensors
    if arg.hx or arg.all:
        tag.setDelegate( HxScaleDelegate())
        tag.hx_scale.enable()
        tag.hx_scale.notify()
        #scale_on  = struct.pack("B", 0x01)
        #tag.scale.write(scale_on)
    #if arg.all:
    #    tag.setDelegate( MovementDelegate())
    #    tag.movement.enable()

    # Some sensors (e.g., temperature, accelerometer) need some time for initialization.
    # Not waiting here after enabling a sensor, the first read value might be empty or incorrect.
    #time.sleep(1.0)
    while True:
	tag.waitForNotifications(0.02)



