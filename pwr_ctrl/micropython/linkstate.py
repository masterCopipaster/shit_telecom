from machine import Pin, Signal
import pins_cfg

links = {"optic 1" : Signal(Pin(pins_cfg.optic1Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "optic 2" : Signal(Pin(pins_cfg.optic2Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "optic 3" : Signal(Pin(pins_cfg.optic3Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "copper 1" : Signal(Pin(pins_cfg.copper1Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "copper 2" : Signal(Pin(pins_cfg.copper2Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "copper 3" : Signal(Pin(pins_cfg.copper3Pin, Pin.IN, Pin.PULL_UP), invert=True),
        "power" : Signal(Pin(pins_cfg.pwrPin, Pin.IN, Pin.PULL_UP), invert=True),
    
}
uplinks = ["optic 1", "copper 1"]
downlinks = ["optic 2", "optic 3", "copper 2", "copper 3"]

link_read_delay = 0.1 # in sec

link_story_length = 5

link_story = {key:[] for key in links.keys()}

def update_link_states():
    global links, link_story_length, link_story
    for linkname in links.keys():
        linksig = links[linkname]
        link_story[linkname].append(linksig.value())
        if len(link_story[linkname]) > link_story_length:
            link_story[linkname].pop(0)
    
