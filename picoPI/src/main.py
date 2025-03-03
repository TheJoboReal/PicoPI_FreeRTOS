import network
import time
import socket

from machine import Pin, Timer, ADC

ssid = 'Kasper-5GHz'
password = '3cc62d6457'

led = Pin("LED", Pin.OUT)
timer = Timer()

def blink(timer):
    led.toggle()
    temperature = ReadTemperature()
    print(temperature)

timer.init(freq=1.0, mode=Timer.PERIODIC, callback=blink)

adcpin = 4
sensor = ADC(adcpin)
  
def ReadTemperature():
    adc_value = sensor.read_u16()
    volt = (3.3/65535) * adc_value
    temperature = 27 - (volt - 0.706)/0.001721
    return round(temperature, 1)

def web_page():
  html = """<html><head><meta name="viewport" content="width=device-width, initial-scale=1"></head>
            <body><h1>Hello World</h1></body></html>
         """
  return html

# if you do not see the network you may have to power cycle
# unplug your pico w for 10 seconds and plug it in again
def ap_mode(ssid, password):
    """
        Description: This is a function to activate AP mode

        Parameters:

        ssid[str]: The name of your internet connection
        password[str]: Password for your internet connection

        Returns: Nada
    """
    # Just making our internet connection
    ap = network.WLAN(network.AP_IF)
    ap.config(essid=ssid, password=password)
    ap.active(True)

    while ap.active() == False:
        pass
    print('AP Mode Is Active, You can Now Connect')
    print('IP Address To Connect to:: ' + ap.ifconfig()[0])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   #creating socket object
    s.bind(('', 80))
    s.listen(5)

    while True:
      conn, addr = s.accept()
      print('Got a connection from %s' % str(addr))
      request = conn.recv(1024)
      print('Content = %s' % str(request))
      response = web_page()
      conn.send(response)
      conn.close()

ap_mode(ssid, password)

