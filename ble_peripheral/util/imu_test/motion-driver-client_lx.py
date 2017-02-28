#!/usr/bin/python

# motion-driver-client.py
# A PC application for use with Motion Driver.
# Copyright 2012 InvenSense, Inc. All Rights Reserved.

import serial, sys, time, string, pygame
from ponycube import *

# Sensor sensitivities
ACCEL_SENS = 16384.0
GYRO_SENS  = 16.375
QUAT_SENS  = 1073741824.0

# Tap direction enums
TAP_X_UP   = 1
TAP_X_DOWN = 2
TAP_Y_UP   = 3
TAP_Y_DOWN = 4
TAP_Z_UP   = 5
TAP_Z_DOWN = 6

# Orientation bits
ORIENTATION_X_UP   = 0x01
ORIENTATION_X_DOWN = 0x02
ORIENTATION_Y_UP   = 0x04
ORIENTATION_Y_DOWN = 0x08
ORIENTATION_Z_UP   = 0x10
ORIENTATION_Z_DOWN = 0x20
ORIENTATION_FLIP   = 0x40
ORIENTATION_ALL    = 0x3F

# Android orientation enums
ANDROID_PORTRAIT    = 0
ANDROID_LANDSCAPE   = 1
ANDROID_R_PORTRAIT  = 2
ANDROID_R_LANDSCAPE = 3

from bluepy.btle import UUID, Peripheral, DefaultDelegate
from collections import deque
import struct

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
        self.movement = MovementSensor(self)
        #self.sync = Sync(self)
        print "geting services"


class MovementSensor(SensorBase):
    svcUUID  = UUID(0xFA00)
    dataUUID = UUID(0xFA05)
    ctrlUUID = UUID(0xFA03)

    def __init__(self, periph):
        SensorBase.__init__(self, periph)

    def enable(self):
        if self.service is None:
           # self.periph.writeCharacteristic(49, struct.pack('<bb', 0x01, 0x00))
            #for uuids,val in  (self.periph.discoverServices()):
            #print type(self.periph.discoverServices())
                #print ("srv: ",srv)
            srvs = self.periph.discoverServices()
            #    print (UUID(0xFA00))
            uuid = UUID(0xFA00)
            self.service = srvs[uuid]

	    print srvs[uuid]
            self.char_descr = self.service.getDescriptors(forUUID=0x2902)[1]
	    self.char_descr.write(struct.pack('<bb', 0x01, 0x00), True)

            #self.service = self.periph.getServiceByUUID(self.svcUUID)
        #if self.ctrl is None:
        #    self.ctrl = self.service.getCharacteristics(self.ctrlUUID) [0]
        if self.data is None:
            self.data = self.service.getCharacteristics(self.dataUUID) [0]

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
        data =  struct.unpack('BBBBBBBBBBBBBBBBBBBB', data)
        self.d_movement.append(data)
    def get_deque(self):
        return self.d_movement


class motion_driver_packet_reader:
    def __init__(self, port, quat_delegate=None, debug_delegate=None, data_delegate=None ):
        self.s = serial.Serial(port,115200)
        self.s.setTimeout(0.1)
        self.s.setWriteTimeout(0.2)

        if quat_delegate:
            self.quat_delegate = quat_delegate
        else:
            self.quat_delegate = empty_packet_delegate()

        if debug_delegate:
            self.debug_delegate = debug_delegate
        else:
            self.debug_delegate = empty_packet_delegate()

        if data_delegate:
            self.data_delegate = data_delegate
        else:
            self.data_delegate = empty_packet_delegate()

        self.packets = []
        self.length = 0
        self.previous = None

    def read(self):
        NUM_BYTES = 23
        MAX_PACKET_TYPES = 8
        p = None

        if self.s.inWaiting():
            c = self.s.read(1)
            if ord(c) == ord('$'):
                # Found the start of a valid packet (maybe).
                c = self.s.read(1)
                if ord(c) < MAX_PACKET_TYPES:
                    d = None
                    p = None
                    if ord(c) == 0 or ord(c) == 1:
                        rs = self.s.read(6)
                        d = data_packet(ord(c),rs)
                    elif ord(c) == 2:
                        rs = self.s.read(16)
                        p = quat_packet(rs)
                        self.quat_delegate.dispatch(p)
                        # Currently, we don't print quaternion data (it's really
                        # meant for the cube display only. If you'd like to
                        # change this behavior, uncomment the following line.
                        #
                        # d = data_packet(ord(c),rs)
                    elif ord(c) == 3:
                        rs = self.s.read(2)
                        d = data_packet(ord(c),rs)
                    elif ord(c) == 4:
                        rs = self.s.read(1)
                        d = data_packet(ord(c),rs)
                    elif ord(c) == 5:
                        rs = self.s.read(8)
                        d = data_packet(ord(c),rs)
                    elif ord(c) == 6:
                        rs = self.s.read(4)
                        d = data_packet(ord(c),rs)
                    if d != None:
                        self.data_delegate.dispatch(d)
                else:
                    print "invalid packet type.."

    def write(self,a):
        self.s.write(a)

    def close(self):
        self.s.close()

    def write_log(self,fname):
        f = open(fname,'w')
        for p in self.packets:
            f.write(p.logfile_line())
        f.close()

class motion_driver_packet_reader_ble:
    def __init__(self, addr, quat_delegate=None, debug_delegate=None, data_delegate=None ):
        #self.s = serial.Serial(port,115200)
        #self.s.setTimeout(0.1)
        #self.s.setWriteTimeout(0.2)
	self.tag = SmartTag(addr, addrType="random")
	self.imu_dele= MovementDelegate()
        self.tag.setDelegate(self.imu_dele)
	self.imu_que = self.imu_dele.get_deque()
        self.tag.movement.enable()
        #self.tag.movement.notify()

        if quat_delegate:
            self.quat_delegate = quat_delegate
        else:
            self.quat_delegate = empty_packet_delegate()

        if debug_delegate:
            self.debug_delegate = debug_delegate
        else:
            self.debug_delegate = empty_packet_delegate()

        if data_delegate:
            self.data_delegate = data_delegate
        else:
            self.data_delegate = empty_packet_delegate()

        self.packets = []
        self.length = 0
        self.previous = None
    def read(self):
        NUM_BYTES = 23
        MAX_PACKET_TYPES = 8
        p = None

        if len(self.imu_que) != 0:
            #c = self.s.read(1)
	    imu_tmp = self.imu_que.popleft()
            print "imu data:", imu_tmp
	    data=[]	
            for i in range(20):
                data.append( chr(imu_tmp[i]))
            #if ord(data[0]) == ord('$'):
                # Found the start of a valid packet (maybe).
                #c = self.s.read(1)
                #if ord(data[1]) < MAX_PACKET_TYPES:
            d = None
            p = None
            #if ord(data[1]) == 2:
                #rs = self.s.read(16)
            rs = data[0:16]
            print rs
            p = quat_packet(rs)
            self.quat_delegate.dispatch(p)
            # Currently, we don't print quaternion data (it's really
            # meant for the cube display only. If you'd like to
            # change this behavior, uncomment the following line.
            #
            # d = data_packet(ord(c),rs)
            if d != None:
                self.data_delegate.dispatch(d)
            #else:
                   # print "invalid packet type.."

# ===========  PACKET DELEGATES  ==========
class packet_delegate(object):
    def loop(self,event):
        print "generic packet_delegate loop w/event",event
    def dispatch(self,p):
        print "generic packet_delegate dispatched",p

class empty_packet_delegate(packet_delegate):
    def loop(self,event):
        pass
    def dispatch(self,p):
        pass

class cube_packet_viewer (packet_delegate):
    def __init__(self):
        self.screen = Screen(480,400,scale=1.5)
        self.cube = Cube(30,60,10)
        self.q = Quaternion(1,0,0,0)
        self.previous = None  # previous quaternion
        self.latest = None    # latest packet (get in dispatch, use in loop)

    def loop(self,event):
        packet = self.latest
        if packet:
            q = packet.to_q().normalized()
            self.cube.erase(self.screen)
            self.cube.draw(self.screen,q)
            pygame.display.flip()
            self.latest = None

    def dispatch(self,p):
        if isinstance(p,quat_packet):
            self.latest = p

class debug_packet_viewer (packet_delegate):
    def loop(self,event):
        pass

    def dispatch(self,p):
        assert isinstance(p,debug_packet);
        p.display()

class data_packet_viewer (packet_delegate):
    def loop(self,event):
        pass

    def dispatch(self,p):
        assert isinstance(p,data_packet);
        p.display()

# =============== PACKETS ================= 
# For 16-bit signed integers.
def two_bytes(d1,d2):
    d = ord(d1)*256 + ord(d2)
    if d > 32767:
        d -= 65536
    return d

# For 32-bit signed integers.
def four_bytes(d1, d2, d3, d4):
    d = ord(d1)*(1<<24) + ord(d2)*(1<<16) + ord(d3)*(1<<8) + ord(d4)
    if d > 2147483648:
        d-= 4294967296
    return d

class debug_packet (object):
    # body of packet is a debug string
    def __init__(self,l):
        sss = []
        for c in l[3:21]:
            if ord(c) != 0:
                sss.append(c)
        self.s = "".join(sss)

    def display(self):
        sys.stdout.write(self.s)

class data_packet (object):
    def __init__(self, type, l):
        self.data = [0,0,0,0]
        self.type = type
        if self.type == 0:     # accel
            self.data[0] = two_bytes(l[0],l[1]) / ACCEL_SENS
            self.data[1] = two_bytes(l[2],l[3]) / ACCEL_SENS
            self.data[2] = two_bytes(l[4],l[5]) / ACCEL_SENS
        elif self.type == 1:   # gyro
            self.data[0] = two_bytes(l[0],l[1]) / GYRO_SENS
            self.data[1] = two_bytes(l[2],l[3]) / GYRO_SENS
            self.data[2] = two_bytes(l[4],l[5]) / GYRO_SENS
        elif self.type == 2:   # quaternion
            self.data[0] = four_bytes(l[0],l[1],l[2],l[3]) / QUAT_SENS
            self.data[1] = four_bytes(l[4],l[5],l[6],l[7]) / QUAT_SENS
            self.data[2] = four_bytes(l[8],l[9],l[10],l[11]) / QUAT_SENS
            self.data[3] = four_bytes(l[12],l[13],l[14],l[15]) / QUAT_SENS
        elif self.type == 3:    # tap
            self.data[0] = ord(l[0])
            self.data[1] = ord(l[1])
        elif self.type == 4:  # Android orient
            self.data[0] = ord(l[0])
        elif self.type == 5:  # pedometer
            self.data[0] = four_bytes(l[0],l[1],l[2],l[3])
            self.data[1] = four_bytes(l[4],l[5],l[6],l[7])
        elif self.type == 6:    # misc
            self.data[0] = ord(l[0])
            if self.data[0] == ord('t'):
                # test event
                self.data[1] = ord(l[1])
        else:   # unsupported
            pass

    def display(self):
        if self.type == 0:
            print 'accel: %7.3f %7.3f %7.3f' % \
                (self.data[0], self.data[1], self.data[2])
        elif self.type == 1:
            print 'gyro: %9.5f %9.5f %9.5f' % \
                (self.data[0], self.data[1], self.data[2])
        elif self.type == 2:
            print 'quat: %7.4f %7.4f %7.4f %7.4f' % \
                (self.data[0], self.data[1], self.data[2], self.data[3])
        elif self.type == 3:
            if self.data[0] == TAP_X_UP:
                s = "+ X"
            elif self.data[0] == TAP_X_DOWN:
                s = "- X"
            elif self.data[0] == TAP_Y_UP:
                s = "+ Y"
            elif self.data[0] == TAP_Y_DOWN:
                s = "- Y"
            elif self.data[0] == TAP_Z_UP:
                s = "+ Z"
            elif self.data[0] == TAP_Z_DOWN:
                s = "- Z"
            print 'Detected %s-axis tap x%d' % (s, self.data[1])
        elif self.type == 4:
            if self.data[0] == ANDROID_PORTRAIT:
                s = "Portrait"
            elif self.data[0] == ANDROID_LANDSCAPE:
                s = "Landscape"
            elif self.data[0] == ANDROID_R_PORTRAIT:
                s = "Reverse portrait"
            elif self.data[0] == ANDROID_R_LANDSCAPE:
                s = "Reverse landscape"
            print 'Screen orientation: %s' % s
        elif self.type == 5:
            print 'Walked %d steps over %d milliseconds.' % \
                (self.data[0], self.data[1])
        elif self.type == 6:
            if self.data[0] == ord('t'):
                if self.data[1] == 7:
                    print 'Self test passed.'
                else:
                    print 'Self test failed.'
            pass
        else:
            print 'what?'

class quat_packet (object):
    def __init__(self, l):
        self.l = l
        self.q0 = four_bytes(l[0],l[1],l[2],l[3]) / QUAT_SENS
        self.q1 = four_bytes(l[4],l[5],l[6],l[7]) / QUAT_SENS
        self.q2 = four_bytes(l[8],l[9],l[10],l[11]) / QUAT_SENS
        self.q3 = four_bytes(l[12],l[13],l[14],l[15]) / QUAT_SENS

    def display_raw(self):
        l = self.l
        print "".join(
            [ str(ord(l[0])), " "] + \
            [ str(ord(l[1])), " "] + \
            [ str(ord(a)).ljust(4) for a in 
                                [ l[2], l[3], l[4], l[5], l[6], l[7], l[8], l[9], l[10] ] ] + \
            [ str(ord(a)).ljust(4) for a in 
                                [ l[8], l[9], l[10] , l[11], l[12], l[13]] ]
            )

    def display(self):
        if 1:
            print "qs " + " ".join([str(s).ljust(15) for s in
                [ self.q0, self.q1, self.q2, self.q3 ]])

    def to_q(self):
        return Quaternion(self.q0, self.q1, self.q2, self.q3)

# =============== MAIN ======================
if __name__ == "__main__":
    if len(sys.argv) == 2:
        #comport = int(sys.argv[1]) - 1
        comport = sys.argv[1]
    else:
        print "usage: " + sys.argv[0] + " port"
        sys.exit(-1)
    pygame.init()
    viewer = cube_packet_viewer()
    debug  = debug_packet_viewer()
    data   = data_packet_viewer()
    reader = motion_driver_packet_reader_ble(comport, 
                quat_delegate = viewer, 
                debug_delegate = debug, 
                data_delegate = data)
    while 1:

        reader.tag.waitForNotifications(0.001)

        event = pygame.event.poll()
        # TODO: Allow exit via keystroke.
        if event.type == pygame.QUIT:
            viewer.close()
            break
        if event.type == pygame.KEYDOWN:
            #reader.write(pygame.key.name(event.key))
            if 'p' == pygame.key.name(event.key):
                #viewer.close()
                reader.tag.disconnect()
                del reader.tag
                sys.exit(-1)


        reader.read()
        viewer.loop(event)
        debug.loop(event)
        data.loop(event)

        # TODO: If system load is too high, increase this sleep time.
        pygame.time.delay(0)
