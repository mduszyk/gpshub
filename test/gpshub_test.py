# run: python3 gpshub_test.py nick friend1,friend2,...friendn

import re
import sys
import time
import getopt

from gpshub_pyclient.client import GpshubClient

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

def read_gps_nmea(file_name):
    fd = open(file_name, 'r')
    for line in fd:
        if re.match('\$GPRMC', line):
            a = line.split(',')
            if len(a[3]) > 0 and len(a[5]) > 0:
                # yield (longitude, latitude)
                yield (int(a[5].replace('.', '')),  int(a[3].replace('.', '')))

def read_gps_simple(file_name):
    fd = open(file_name, 'r')
    for line in fd:
            a = line.split(' ')
            # yield (longitude, latitude)
            yield (int(a[1]),  int(a[0]))

def handle_cmd_pkg(pkg):
    print('CMDPKG', pkg)

def handle_gps_pkg(pkg):
    print('GPSPKG', pkg['uid'], pkg['lat'], pkg['lon'])

if __name__== "__main__":
    parse_opts()
    
    print("Starting gpshub test...")
    print()
    print_params()
    print()
    
    gpshub = GpshubClient(nick, buddies, host, port_cmd, port_gps)
    gpshub.external_cmd_pkg_handler = handle_cmd_pkg
    gpshub.external_gps_pkg_handler = handle_gps_pkg
    gpshub.connect()

    try:
        # send positions to gpshub
        for pos in read_gps_nmea(gps_file):
        #for pos in read_gps_simple(gps_file):
            time.sleep(1.0)
            gpshub.gps_channel.send_pos(pos)
    except KeyboardInterrupt:
        gpshub.gps_listener.stop()
        gpshub.cmd_listener.stop()
        print()
