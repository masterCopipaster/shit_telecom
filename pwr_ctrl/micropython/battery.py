from machine import ADC, Pin

DIVIDER = 10

batPIN = 2
batADC = None

def bat_init():
    global batADC, batPIN
    batADC = ADC(batPIN, atten = ADC.ATTN_11DB)
    batADC.width(ADC.WIDTH_12BIT)
    print("battery init on pin {} with divider {}".format(batPIN, DIVIDER))
    pass
    
def read_voltage():
    global batADC
    return batADC.read_uv() * DIVIDER / 1e6 
