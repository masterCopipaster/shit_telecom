from machine import ADC, Pin

for pin in range(50):
    print("Trying pin {}".format(pin))
    try:
        adc = ADC(pin, atten = ADC.ATTN_11DB)
        print("good {}".format(adc.read_uv()))
    except:
        print("huita")