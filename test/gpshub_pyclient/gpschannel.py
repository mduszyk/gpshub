import socket
import struct
from threading import Thread
from ctypes import c_uint16, c_uint32

class GpsChannel:
    
    def __init__(self, host, port, userid):
        self.host = host
        self.port = port
        self.userid = userid
        self.uid = bytes(c_uint32(socket.htonl(self.userid)))
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.connect((self.host, self.port))
        # default timeout in secs
        #self.sock.settimeout(1.0)
    
    def init_udp(self, token):
        tkn = bytes(c_uint32(socket.htonl(token)))
        ba = bytearray()
        ba.extend(self.uid)
        ba.extend(tkn)
        self.sock.send(ba)
    
    def send_pos(self, pos):
        """ pos is a tuple: (longitude, latitude) """
        ba = bytearray()
        ba.extend(self.uid)
        ba.extend(bytes(c_uint32(socket.htonl(pos[0]))))
        ba.extend(bytes(c_uint32(socket.htonl(pos[1]))))
        # send altitude optionally
        if len(pos) > 2:
            ba.extend(bytes(c_uint32(socket.htonl(pos[2]))))
        self.sock.send(ba)
    
    def read(self):
        try:
            pkg_bytes = self.sock.recv(4096)
            if pkg_bytes is not None and len(pkg_bytes) > 0:
                return self.__decode_pkg(pkg_bytes)
        except socket.timeout:
            pass
        return None
            
    def __decode_pkg(self, pkg_bytes):
        pkg = dict()
        pkg['bytes'] = pkg_bytes
        pkg['uid'] = struct.unpack('!I', pkg_bytes[0:4])[0]
        pkg['lon'] = struct.unpack('!I', pkg_bytes[4:8])[0]
        pkg['lat'] = struct.unpack('!I', pkg_bytes[8:12])[0]

        return pkg


class GpsChannelListener(Thread):

    def __init__(self, gps_channel, nick):
        Thread.__init__(self)
        self.gps_channel = gps_channel
        self.__read = True
        self.nick = nick
        self._observers = []
    
    def add_observer(self, observer):
        self._observers.append(observer)

    def run(self):
        while self.__read:
            pkg = self.gps_channel.read()
            if self.__read and pkg is not None:
                for observer in self._observers:
                    observer(pkg)
    
    def stop(self):
        self.__read = False
        self.gps_channel.sock.shutdown(socket.SHUT_RDWR)
