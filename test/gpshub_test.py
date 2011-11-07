# run: python3 gpshub_test.py nick friend1,friend2,...friendn

import re
import sys
import time
import getopt
import threading

from gpshub_pyclient.cmdchannel import CmdChannel, CmdChannelListener
from gpshub_pyclient.gpschannel import GpsChannel, GpsChannelListener


nick = 'foobar'
buddies = 'foo,bar,baz'
host = 'localhost'
port_cmd = 9990
port_gps = 9991
gps_file = 'gps_data/Rynek-2009-04-15.09.55.gps'


def print_params():
    print("Nick:", nick)
    print("Buddies:", buddies)
    print("Gps file:", gps_file)
    print("Host:", host)
    print("TCP:", port_cmd)
    print("UDP:", port_gps)

def parse_opts():
    global nick, buddies, host, port_cmd, port_gps, gps_file
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:t:u:f:", \
            ["host=", "tcp=", "udp=", "file="])
    except(getopt.GetoptError, err):
        print(str(err))
        sys.exit(2)
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

def read_gps_positions(file_name):
    fd = open(file_name, 'r')
    for line in fd:
        if re.match('\$GPRMC', line):
            a = line.split(',')
            if len(a[3]) > 0 and len(a[5]) > 0:
                # yield (longitude, latitude)
                yield (int(a[5].replace('.', '')),  int(a[3].replace('.', '')))

class GpshubFakeClient():
    
    def __init__(self, nick, buddies, host, port_cmd, port_gps):
        self.nick = nick
        self.buddies = buddies
        self.host = host
        self.port_cmd = port_cmd
        self.port_gps = port_gps
        self.userid = None
        self.token = None
        self.cmd_channel = None
        self.cmd_listener = None
        self.gps_channel = None
        self.gps_listener = None
        # initialize condition variable
        self.cv = threading.Condition()
    
    def start_cmd_channel(self):
        self.cmd_channel = CmdChannel(self.host, self.port_cmd)
        
        self.cmd_listener = CmdChannelListener(self.cmd_channel)
        self.cmd_listener.daemon = True
        self.cmd_listener.add_observer(self.handle_cmd_pkg)
        self.cmd_listener.start()
        
        self.cmd_channel.register_nick(self.nick)
        
    def start_gps_channel(self):
        self.gps_channel = GpsChannel(self.host, self.port_gps, self.userid)
        
        self.gps_listener = GpsChannelListener(self.gps_channel, self.nick)
        self.gps_listener.daemon = True
        self.gps_listener.add_observer(self.handle_gps_pkg)
        self.gps_listener.start()
        
        # TODO this goes through UDP so it should be invoked
        # in loop until init ack received
        self.gps_channel.init_udp(self.token)

    def handle_cmd_pkg(self, pkg):
        print('CMDPKG', pkg)
        
        if pkg['type'] == CmdChannel.REGISTER_NICK_ACK:
            if pkg['status'] == 1:
                self.userid = pkg['userid']
                self.cmd_channel.add_buddies(buddies)
                if self.token is not None:
                    self.start_gps_channel()
            else:
                print('ERROR couldn\'t register nick')
                exit(1)
        
        if pkg['type'] == CmdChannel.INITIALIZE_UDP:
            self.token = pkg['token']
            if self.userid is not None:
                self.start_gps_channel()
        
        if pkg['type'] == CmdChannel.INITIALIZE_UDP_ACK:
            if pkg['status'] == 1:
                self.cv.acquire()
                self.cv.notify()
                self.cv.release()

    def handle_gps_pkg(self, pkg):
        print('GPSPKG', pkg['uid'], pkg['lon'], pkg['lat'])


if __name__== "__main__":
    parse_opts()
    
    print("Starting gpshub test...")
    print()
    print_params()
    print()
    
    hub_client = GpshubFakeClient(nick, buddies, host, port_cmd, port_gps)
    hub_client.start_cmd_channel()

    # wait for gps channel to be initialized
    hub_client.cv.acquire()
    hub_client.cv.wait()
    hub_client.cv.release()

    try:
        # send positions to gpshub
        for pos in read_gps_positions(gps_file):
            time.sleep(0.6)
            hub_client.gps_channel.send_pos(pos)
    except KeyboardInterrupt:
        hub_client.gps_listener.stop()
        hub_client.cmd_listener.stop()
        print()
