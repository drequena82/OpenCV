from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import time
import serial
import numpy as np

arduino=serial.Serial('/dev/ttyUSB0',1843200)

camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=(320, 240))

display_window = cv2.namedWindow("Faces")

face_cascade = cv2.CascadeClassifier('/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml')

time.sleep(0.1)

lcd_width = 84
lcd_height = 48
lcd_x = 0
lcd_y = 0
lcd_w = 0
lcd_h = 0

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

    image = frame.array

    #FACE DETECTION STUFF
    gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 5)

    height = np.size(image, 0)
    width = np.size(image, 1)
    count = 0
    for (x,y,w,h) in faces:

        lcd_x = int(float(x) * float(lcd_width) / float(width))
        lcd_y = int(float(y) * float(lcd_height) / float(height))
        lcd_w = int(float(w) * float(lcd_width) / float(width))
        lcd_h = int(float(h) * float(lcd_height) / float(height))

        if count == 0:
        	cv2.rectangle(image,(x,y),(x+w,y+h),(255,0,0),2)

            print ('heigth' + str(height) + 'width '+ str(width) + ' x: ' + str(x) + ' y: ' + str(y) + ' w: ' + str(w) + ' h:' + str(h))

            print ('heigth' + str(lcd_height) + 'width ' + str(lcd_width) + ' x: ' + str(lcd_x) + ' y: ' + str(lcd_y) + ' w: ' + str(lcd_w) + ' h:' + str(lcd_h))

    	    arduino.write(str(lcd_x) + '_' + str(lcd_y) + '_' + str(lcd_w) + '_' + str(lcd_h))
	    count = count + 1

    #DISPLAY TO WINDOW
    cv2.imshow("Faces", image)
    key = cv2.waitKey(1)

    rawCapture.truncate(0)

    if key == 27:
        camera.close()
        cv2.destroyAllWindows()
        break
