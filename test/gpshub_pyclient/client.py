import time
import threading

from gpshub_pyclient.cmdchannel import CmdChannel, CmdChannelListener
from gpshub_pyclient.gpschannel import GpsChannel, GpsChannelListener

class GpshubClient():
    
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
        self.cv_gps_channel = threading.Condition()
        self.udp_initialized = False
        self.external_cmd_pkg_handler = None
        self.external_gps_pkg_handler = None
    
    def _connect_cmd_channel(self):
        self.cmd_channel = CmdChannel(self.host, self.port_cmd)
        
        self.cmd_listener = CmdChannelListener(self.cmd_channel)
        self.cmd_listener.daemon = True
        self.cmd_listener.add_observer(self._handle_cmd_pkg)
        self.cmd_listener.start()
        
        self.cmd_channel.register_nick(self.nick)
        
    def _connect_gps_channel(self):
        self.gps_channel = GpsChannel(
            self.host, self.port_gps, self.userid)
        
        self.gps_listener = GpsChannelListener(
            self.gps_channel, self.nick)
        self.gps_listener.daemon = True
        self.gps_listener.add_observer(self._handle_gps_pkg)
        self.gps_listener.start()
        
        threading.Thread(target=self._initialize_udp).start()
    
    def _initialize_udp(self):
        """ Method init_udp must be called in loop until ack arrives
            because it goes throudht UDP and packet might be lost.
        """
        while not self.udp_initialized:
            self.gps_channel.init_udp(self.token)
            time.sleep(0.33)

    def _handle_cmd_pkg(self, pkg):
        
        if pkg['type'] == CmdChannel.REGISTER_NICK_ACK:
            if pkg['status'] == 1:
                self.userid = pkg['userid']
                self.cmd_channel.add_buddies(self.buddies)
                if self.token is not None:
                    self._connect_gps_channel()
            else:
                print('ERROR couldn\'t register nick')
                exit(1)
        
        if pkg['type'] == CmdChannel.INITIALIZE_UDP:
            self.token = pkg['token']
            if self.userid is not None:
                self.udp_initialized = False
                self._connect_gps_channel()
        
        if pkg['type'] == CmdChannel.INITIALIZE_UDP_ACK:
            if pkg['status'] == 1:
                self.udp_initialized = True
                self.cv_gps_channel.acquire()
                self.cv_gps_channel.notify()
                self.cv_gps_channel.release()
        
        if self.external_cmd_pkg_handler is not None:
            self.external_cmd_pkg_handler(pkg)

    def _handle_gps_pkg(self, pkg):
        if self.external_gps_pkg_handler is not None:
            self.external_gps_pkg_handler(pkg)
        
    def connect(self):
        self._connect_cmd_channel()
        # wait for gps channel to be initialized
        self.cv_gps_channel.acquire()
        self.cv_gps_channel.wait()
        self.cv_gps_channel.release()
