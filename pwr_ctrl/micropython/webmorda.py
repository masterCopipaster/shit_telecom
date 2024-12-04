from micropyserver import MicroPyServer
import json
import utils
import _thread
import battery
import linkstate

server = None
request_counter = 0

def say_hello(request):
    global server, request_counter
    server.send("ty eblan {}!".format(request_counter))
    request_counter += 1
    
def show_battery(request):
    global server
    server.send("battery voltage {}".format(battery.read_voltage()))

def show_linkstates(request):
    global server
    linkstates = linkstate.make_solution()
    server.send("\n".join(["{} {}".format(name, state) for (name, state) in linkstates.items()]))

def init():
    global server
    server = MicroPyServer(host="0.0.0.0", port=80)
    server.add_route("/hello", say_hello)
    server.add_route("/battery", show_battery)
    server.add_route("/links", show_linkstates)
    #server.start()
    _thread.start_new_thread(server.start, ())
    
def webmorda_stop():
    server.stop()

    

    