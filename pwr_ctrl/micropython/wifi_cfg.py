import network
import time
import led

#connect_order = [("STA", "boyare", "beychelom", 3), ("STA", "lehagovnalepeha", "mobila90", 3), ("STA", None, "", 10), ("AP", "MMM_HUITA", None, -1)]
#connect_order = [("STA", None, None, 5), ("AP", "MMM_HUITA", "mobila90", 30), ("AP", "MMM_HUITA", None, -1)]
#connect_order = [("AP", "MMM_HUITA", None, -1)]
connect_order = [("STA", "lehagovnalepeha", "mobila90", -1)]
#connect_order = [("STA", "P2", None, -1)]

WLAN = None
mod = ['OPEN', 'WEP', 'WPA-PSK' 'WPA2-PSK4', 'WPA/WPA2-PSK']

def try_given_ssid(WLAN, ssid, pswd, timeout):
    print("Trying to connect ssid:{} key:{} timeout:{}".format(ssid, pswd, timeout))
    
    found = False
    
    for (sc_ssid, bssid, channel, RSSI, authmode, hidden) in WLAN.scan():
        if ssid == sc_ssid.decode("utf-8"):
            found = True
            break
        
    if not found:
        print("NIHT")
        return False
        
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
    else:
        res = True
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
                res = try_given_ssid(WLAN, ssid.decode("utf-8"), pswd, timeout)
                if res:
                    break
    if not res:
        WLAN.active(False)
    return res
    
pm_map = {network.WLAN.PM_PERFORMANCE : "WLAN.PM_PERFORMANCE", network.WLAN.PM_POWERSAVE : "WLAN.PM_POWERSAVE", network.WLAN.PM_NONE : "WLAN.PM_NONE"}

status_map = {network.STAT_IDLE : "STAT_IDLE", network.STAT_CONNECTING : "STAT_CONNECTING", network.STAT_WRONG_PASSWORD : "STAT_WRONG_PASSWORD",
    network.STAT_NO_AP_FOUND : "STAT_NO_AP_FOUND", network.STAT_GOT_IP : "STAT_GOT_IP"
}

def get_wlan_config():
    global WLAN
    return (WLAN.config("ssid"), WLAN.ipconfig("addr4"))#, status_map[WLAN.status()])#, pm_map[WLAN.config("pm")])#, WLAN.status("rssi"))

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

import _thread

def wifi_cfg_thread():
    run_connect_order()
    while 1:
        if WLAN.active():
            if WLAN.isconnected():
                led.led_slow()
            else:
                led.led_fast()
        else:
            led.led_off()
        print(get_wlan_config())
        time.sleep(1)

def init():
    _thread.start_new_thread(wifi_cfg_thread, ())
    

init()

