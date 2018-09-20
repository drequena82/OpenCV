#!/usr/bin/python
import serial
import time

arduino=serial.Serial('/dev/ttyUSB0',9600)
cadena=''
 
while True:
      var = raw_input("Introduzca  un Comando: ")
      arduino.write(var + '\n')
      time.sleep(0.1)
      while arduino.inWaiting() > 0:
            cadena += arduino.readline()
            print cadena.rstrip('\n')
            cadena = ''
arduino.close()