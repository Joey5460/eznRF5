#!/usr/bin/env python
from bluepy.btle import UUID, Peripheral, DefaultDelegate
import struct
import math
from collections import deque
import serial, sys, time, string, pygame


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
        self.sync = Sync(self)
        print "geting services"

class MovementSensor(SensorBase):
    svcUUID  = UUID(0xFA00)
    dataUUID = UUID(0xFA04)
    ctrlUUID = UUID(0xFA03)

    def __init__(self, periph):
        SensorBase.__init__(self, periph)

    def enable(self):
        if self.service is None:
            self.periph.writeCharacteristic(49, struct.pack('<bb', 0x01, 0x00))
            #for uuids,val in  (self.periph.discoverServices()):
            #print type(self.periph.discoverServices())
                #print ("srv: ",srv)
            srvs = self.periph.discoverServices()
            #    print (UUID(0xFA00))
            uuid = UUID(0xFA00)
            self.service = srvs[uuid]

            print srvs[uuid]

            #self.service = self.periph.getServiceByUUID(self.svcUUID)
        if self.ctrl is None:
            self.ctrl = self.service.getCharacteristics(self.ctrlUUID) [0]
        if self.data is None:
            self.data = self.service.getCharacteristics(self.dataUUID) [0]
        if self.sensorOn is not None:
            self.ctrl.write(self.sensorOn,withResponse=True)

    def read(self):
        '''Returns (x_accel, y_accel, z_accel) in units of g'''
        x_y_z = struct.unpack('HHHHHHHHHH', self.data.read())
        return tuple([ (val) for val in x_y_z ])

class MovementDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
        self.d_movement = deque()

    def handleNotification(self, hnd, data):
        # NB: only one source of notifications at present
        # so we can ignore 'hnd'.
        data =  struct.unpack('HHhhhhhHHH', data)
        ls = [(data[0] + data[1]*256*256), (data[2], data[3], data[4]),(data[6],data[7], data[8])]
        with open ('up.dat', 'a+') as fd:
            print ls[0], data[2]/100.0,data[3]/100.0,data[4]/100.0  
            print>>fd, data[3]/100.0, data[4]/100.0, data[5]/100.0, data[6]/100.0, (math.atan2(data[3],data[5])*57.3+180)%360
        self.d_movement.append(ls)
    def get_deque(self):
        return self.d_movement

class Sync(SensorBase):
    def __init__(self, periph):
        SensorBase.__init__(self, periph)

        self.svcUUID  = UUID(0xFA40)
        self.ctrlUUID = UUID(0xFA43)
        self.dataUUID = UUID(0xFA45)
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

class SyncDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
        self.sync_deque = deque()
	self.fd_sync = open('sync.dat', 'w')

    def handleNotification(self, hnd, data):
        data_ex =  struct.unpack('BBBBBBBBBBBBBBBBBBBB', data)
        print "sync:",":".join("{:02x}".format(c) for c in data_ex) 
	newFileByteArray = bytearray(data[4:])
	self.fd_sync.write(newFileByteArray)
	if (0x20 == data_ex[1]):
            print "sync done"
	    self.fd_sync.close()
            sync_clear  = struct.pack("B", 0x02)
            tag.sync.write(sync_clear)
        #self.sync_deque.append(data)

    def get_deque(self):
        return self.sync_deque



if __name__ == "__main__":
    import time
    import sys
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('host', action='store',help='MAC of BT device')
    parser.add_argument('-n', action='store', dest='count', default=0,
            type=int, help="Number of times to loop data")
    parser.add_argument('-t',action='store',type=float, default=5.0, help='time between polling')
    parser.add_argument('-A','--accelerometer', action='store_true',
            default=False)
    parser.add_argument('-G','--gyroscope', action='store_true', default=False)
    parser.add_argument('-S','--sync', action='store_true', default=False)
    parser.add_argument('--all', action='store_true', default=False)

    arg = parser.parse_args(sys.argv[1:])

    print('Connecting to ' + arg.host)
    tag = SmartTag(arg.host, addrType="random")

    # Enabling selected sensors
    if arg.sync or arg.all:
        tag.setDelegate( SyncDelegate())
        tag.sync.enable()
        tag.sync.notify()
        sync_on  = struct.pack("B", 0x01)
        tag.sync.write(sync_on)
    #if arg.all:
    #    tag.setDelegate( MovementDelegate())
    #    tag.movement.enable()

    # Some sensors (e.g., temperature, accelerometer) need some time for initialization.
    # Not waiting here after enabling a sensor, the first read value might be empty or incorrect.
    #time.sleep(1.0)
    pygame.init()
    while True:
       #tag.waitForNotifications(arg.t)
	event = pygame.event.poll()
        # TODO: Allow exit via keystroke.
        if event.type == pygame.QUIT:
            viewer.close()
            break
        if event.type == pygame.KEYDOWN:
            #reader.write(pygame.key.name(event.key))
            if 'p' == pygame.key.name(event.key):
                #viewer.close()
                tag.disconnect()
                del tag
                sys.exit(-1)
	tag.waitForNotifications(0.02)



