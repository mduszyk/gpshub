# run: python3 gpshub_test.py nick friend1,friend2,...friendn

import re
import sys
import time
import getopt

from gpshub_pyclient.cmdchannel import CmdChannel, CmdChannelListener
from gpshub_pyclient.gpschannel import GpsChannel, GpsChannelListener


def read_gps_positions(file_name):
    fd = open(file_name, 'r')
    for line in fd:
        if re.match('\$GPRMC', line):
            a = line.split(',')
            if len(a[3]) > 0 and len(a[5]) > 0:
                # yield tuple (longitude, latitude)
                yield (int(a[5].replace('.', '')),  int(a[3].replace('.', '')))


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
    
        
    cmd_channel = CmdChannel(host, port_cmd)
    # set socket timeout to turn off complete blocking on recv
    cmd_channel.sock.settimeout(1.0)
    
    cmd_channel.register_nick(nick)
    pkg = cmd_channel.read()
    if pkg['type'] == CmdChannel.REGISTER_NICK_ACK and pkg['status'] == 1:
        userid = pkg['userid']
        print('CMD', pkg)
    else:
        print('ERROR couldn\'t register nick')
        exit(1)
    
    pkg = cmd_channel.read()
    if pkg['type'] == CmdChannel.INITIALIZE_UDP:
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
