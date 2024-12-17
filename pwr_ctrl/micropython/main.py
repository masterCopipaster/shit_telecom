import webmorda
import battery
import linkstate
import pwr_ctrl
import led

linkstate.init()
battery.init()
pwr_ctrl.init()
webmorda.init()
led.init()
led.led_on()
import wifi_cfg



