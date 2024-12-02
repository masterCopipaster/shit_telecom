from micropyserver import MicroPyServer
import json
import utils
import _thread
import battery

server = None
request_counter = 0

def say_hello(request):
    global server, request_counter
    server.send("ty eblan {}!".format(request_counter))
    request_counter += 1
    
def show_battery(request):
    global server
    server.send("battery voltage {}".format(battery.read_voltage()))

def init():
    global server
    server = MicroPyServer(host="0.0.0.0", port=80)
    server.add_route("/hello", say_hello)
    server.add_route("/battery", show_battery)
    #server.start()
    _thread.start_new_thread(server.start, ())
    
def webmorda_stop():
    server.stop()

    

    