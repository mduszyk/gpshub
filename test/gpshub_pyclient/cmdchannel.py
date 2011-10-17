import socket
import struct
from threading import Thread
from ctypes import c_uint16, c_uint32

class CmdChannel:
    
    MAX_PKG_LEN = 512
    
    REGISTER_NICK = 1;
    ADD_BUDDIES = 2;
    REMOVE_BUDDIES = 3;
    REGISTER_NICK_ACK = 101;
    ADD_BUDDIES_ACK = 102;
    INITIALIZE_UDP = 105;
    INITIALIZE_UDP_ACK = 106;
    BUDDIES_IDS = 150;
    
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        # default timeout in secs
        #self.sock.settimeout(1.0)
        
    def _send(self, pkg_type, data):
        data_bytes = bytes(data, 'utf-8')
        pkg_len = 3 + len(data_bytes)
        buf = bytearray()
        buf.append(pkg_type)
        buf.extend(bytes(c_uint16(socket.htons(pkg_len))))
        buf.extend(data_bytes)
        self.sock.send(buf)
    
    def register_nick(self, nick):
        self._send(CmdChannel.REGISTER_NICK, nick)
        
    def add_buddies(self, csv):
        self._send(CmdChannel.ADD_BUDDIES, csv)
    
    def remove_buddies(self, csv):
        self._send(CmdChannel.REMOVE_BUDDIES, csv)
    
    def read(self):
        try:
            header = self.sock.recv(3)
            if header is not None and len(header) > 0:
                pkg = dict()
                pkg['header'] = header
                pkg['type'] = header[0]
                pkg['len'] = struct.unpack('!H', header[1:3])[0]
                pkg['data'] = self.sock.recv(pkg['len'] - 3)
                return self.__decode_pkg(pkg)
        except socket.timeout:
            pass
        return None
    
    def __decode_pkg(self, pkg):
        {CmdChannel.REGISTER_NICK_ACK: self.__decode_nick_ack,
         CmdChannel.BUDDIES_IDS: self.__decode_buddies_ids,
         CmdChannel.INITIALIZE_UDP: self.__decode_initialize_udp,
         CmdChannel.INITIALIZE_UDP_ACK: self.__decode_initialize_udp_ack,
        }.get(pkg['type'], self.__unknown_pkg)(pkg)

        return pkg
        
    def __unknown_pkg(self, pkg):
        pkg['unknown'] = 1
        
    def __decode_nick_ack(self, pkg):
        pkg['status'] = pkg['data'][0]
        if pkg['status'] == 1:
            pkg['userid'] = struct.unpack('!I', pkg['data'][1:5])[0]
    
    def __decode_buddies_ids(self, pkg):
        offset = 0
        buddies = dict();
        data_len = pkg['len'] - 3
        while offset < data_len:
            uid = struct.unpack('!I', pkg['data'][offset : offset + 4])[0]
            offset += 4
            i = offset
            while i < data_len and pkg['data'][i] != 0:
                i += 1
            n = i - offset
            buddy = str(struct.unpack('!' + str(n) + 's', pkg['data'][offset:i])[0], 'latin1')
            offset = i + 1
            buddies[uid] = buddy
        pkg['buddies'] = buddies

    def __decode_initialize_udp(self, pkg):
        pkg['token'] = struct.unpack('!I', pkg['data'][0:4])[0]
	
    def __decode_initialize_udp_ack(self, pkg):
        pkg['status'] = pkg['data'][0]


class CmdChannelListener(Thread):
    
    def __init__(self, cmd_channel):
        Thread.__init__(self)
        self.cmd_channel = cmd_channel
        self.__read = True
        self._observers = []

    def add_observer(self, observer):
        self._observers.append(observer)

    def run(self):
        while self.__read:
            pkg = self.cmd_channel.read()
            if self.__read and pkg is not None:
                for observer in self._observers:
                    observer(pkg)

    
    def stop(self):
        self.__read = False
        self.cmd_channel.sock.shutdown(socket.SHUT_RDWR)
