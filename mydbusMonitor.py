import dbus, gobject
from dbus.mainloop.glib import DBusGMainLoop

def msg_cb(msg):
    print msg

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SessionBus()
    bus.add_signal_receiver(msg_cb, dbus_interface="com.Secure.PassGenService")

    mainloop = gobject.MainLoop()
    mainloop.run()