import network
import time

connect_order = [("STA", "boyare", "beychelom", 3), ("STA", None, None, 5), ("AP", "MMM_HUITA", None, -1)]
#connect_order = [("STA", None, None, 5), ("AP", "MMM_HUITA", "mobila90", 30), ("AP", "MMM_HUITA", None, -1)]

WLAN = None
mod = ['OPEN', 'WEP', 'WPA-PSK' 'WPA2-PSK4', 'WPA/WPA2-PSK']

def try_given_ssid(WLAN, ssid, pswd, timeout):
    print("Trying to connect ssid:{} key:{} timeout:{}".format(ssid, pswd, timeout))
    WLAN.connect(ssid, pswd)
    if timeout > 0:
        res = False
        starttime = time.time()
        while (time.time() - starttime) < timeout:
            time.sleep(0.1)
            res = WLAN.isconnected()
            if res:
                break
        if res:
            print("AGA")
        else:
            print("NEA")
        return res
    return True


def try_ap_connection(ssid, pswd, timeout):
    global WLAN
    print("Trying to create AP ssid:{} key:{} timeout:{}".format(ssid, pswd, timeout))
    WLAN = network.WLAN(network.WLAN.IF_AP) # create access-point interface
    WLAN.active(True)       # activate the interface
    if pswd != None:
        WLAN.config(ssid=ssid, key=pswd) # set the SSID of the access point
    else:
        WLAN.config(ssid=ssid)
    res = False
    if timeout > 0:
        res = False
        starttime = time.time()
        while (time.time() - starttime) < timeout:
            time.sleep(0.1)
            res = WLAN.isconnected()
            if res:
                break
        print("".format("aga" if res else "nea"))
    if not res:
        WLAN.active(False)
    return res
    
    
def try_sta_connection(ssid, pswd, timeout):
    global WLAN
    WLAN = network.WLAN(network.WLAN.IF_STA) # create station interface
    WLAN.active(True)       # activate the interface
    res = False
    if ssid != None:
        res = try_given_ssid(WLAN, ssid, pswd, timeout)
    else:
        for (ssid, bssid, channel, RSSI, authmode, hidden) in WLAN.scan():
            if authmode == 0:
                res = try_given_ssid(WLAN, ssid, pswd, timeout)
                if res:
                    break
    if not res:
        WLAN.active(False)
    return res
    

def get_wlan_config():
    global WLAN
    return (WLAN.config("ssid"), WLAN.ipconfig("addr4"))

def run_connect_order():
    global WLAN
    for (mode, ssid, pswd, timeout) in connect_order:
        if mode == "STA":
            if try_sta_connection(ssid, pswd, timeout):
                break
        elif mode == "AP":
            if try_ap_connection(ssid, pswd, timeout):
                break
        else:
            continue

run_connect_order()
print(get_wlan_config())

