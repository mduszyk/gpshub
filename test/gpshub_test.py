# run: python3 gpshub_test.py nick friend1,friend2,...friendn

import re
import sys
import socket
import struct
import time
import getopt
from threading import Thread
from ctypes import c_uint16, c_uint32


def read_gps_positions(file_name):
    fd = open(file_name, 'r')
    for line in fd:
        if re.match('\$GPRMC', line):
            a = line.split(',')
            if len(a[3]) > 0 and len(a[5]) > 0:
                # yield tuple (longitude, latitude)
                yield (int(a[5].replace('.', '')),  int(a[3].replace('.', '')))

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

class CommandChannel:
    
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
        self._send(CommandChannel.REGISTER_NICK, nick)
        
    def add_buddies(self, csv):
        self._send(CommandChannel.ADD_BUDDIES, csv)
    
    def remove_buddies(self, csv):
        self._send(CommandChannel.REMOVE_BUDDIES, csv)
    
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
        {CommandChannel.REGISTER_NICK_ACK: self.__decode_nick_ack,
         CommandChannel.BUDDIES_IDS: self.__decode_buddies_ids,
         CommandChannel.INITIALIZE_UDP: self.__decode_initialize_udp,
         CommandChannel.INITIALIZE_UDP_ACK: self.__decode_initialize_udp_ack,
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

class GpsChannelListener(Thread):

    def __init__(self, gps_channel, nick):
        Thread.__init__(self)
        self.gps_channel = gps_channel
        self.__read = True
        self.nick = nick

    def run(self):
        while self.__read:
            pkg = self.gps_channel.read()
            if self.__read and pkg is not None:
                print(self.nick, self.gps_channel.userid,
                    'PKG:', pkg['uid'], pkg['lon'], pkg['lat'])
    
    def stop(self):
        self.__read = False
        self.gps_channel.sock.shutdown(socket.SHUT_RDWR)

class CmdChannelListener(Thread):
    
    def __init__(self, cmd_channel):
        Thread.__init__(self)
        self.cmd_channel = cmd_channel
        self.__read = True

    def run(self):
        while self.__read:
            pkg = self.cmd_channel.read()
            if self.__read and pkg is not None:
                print('CMD', pkg)
    
    def stop(self):
        self.__read = False
        self.cmd_channel.sock.shutdown(socket.SHUT_RDWR)


if __name__== "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:t:u:f:", \
            ["host=", "tcp=", "udp=", "file="])
    except(getopt.GetoptError, err):
        print(str(err))
        sys.exit(2)

    nick = 'foobar'
    buddies = 'foo,bar,baz'
    host = 'localhost'
    port_cmd = 9990
    port_gps = 9991
    gps_file = 'gps_data/Rynek-2009-04-15.09.55.gps'
    
    if len(args) > 0:
        nick = args[0]
    if len(args) > 1:
       buddies = args[1]
    for o, a in opts:
        if o in ("-h", "--host"):
            host = a
        elif o in ("-t", "--tcp"):
            port_cmd = int(a)
        elif o in ("-u", "--udp"):
            port_gps = int(a)
        elif o in ("-f", "--file"):
            gps_file = a
        else:
            assert False, "unhandled option"
    
    print("Testing gpshub...")
    print()
    print("Nick:", nick)
    print("Buddies:", buddies)
    print("Gps file:", gps_file)
    print("Host:", host)
    print("TCP:", port_cmd)
    print("UDP:", port_gps)
    print()
    
        
    cmd_channel = CommandChannel(host, port_cmd)
    # set socket timeout to turn off complete blocking on recv
    cmd_channel.sock.settimeout(1.0)
    
    cmd_channel.register_nick(nick)
    pkg = cmd_channel.read()
    if pkg['type'] == CommandChannel.REGISTER_NICK_ACK and pkg['status'] == 1:
        userid = pkg['userid']
        print('CMD', pkg)
    else:
        print('ERROR couldn\'t register nick')
        exit(1)
    
    pkg = cmd_channel.read()
    if pkg['type'] == CommandChannel.INITIALIZE_UDP:
        token = pkg['token']
        print('CMD', pkg)
    else:
        print('ERROR server haven\'t sent init udp token')
        exit(1)
	
    cmd_listener = CmdChannelListener(cmd_channel)
    cmd_listener.start()
    
    cmd_channel.add_buddies(buddies)

    gps_channel = GpsChannel(host, port_gps, userid)
    gps_channel.init_udp(token)
    
    # receive positions
    gps_listener = GpsChannelListener(gps_channel, nick)
    #gps_listener.daemon = True
    gps_listener.start()
    
    #cmd_channel.remove_buddies('bar,baz,boz')

    try:
        # send positions to gpshub
        for pos in read_gps_positions(gps_file):
            time.sleep(0.6)
            gps_channel.send_pos(pos)
    except KeyboardInterrupt:
        gps_listener.stop()
        cmd_listener.stop()
        print()
