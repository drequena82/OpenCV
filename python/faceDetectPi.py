from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import time
import serial

arduino=serial.Serial('/dev/ttyUSB0',1843200)

camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=(320, 240))

display_window = cv2.namedWindow("Faces")

face_cascade = cv2.CascadeClassifier('/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml')

time.sleep(0.1)

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

    image = frame.array

    #FACE DETECTION STUFF
    gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 5)
    for (x,y,w,h) in faces:
        cv2.rectangle(image,(x,y),(x+w,y+h),(255,0,0),2)
    	arduino.write('envio arduino')

    #DISPLAY TO WINDOW
    cv2.imshow("Faces", image)
    key = cv2.waitKey(1)

    rawCapture.truncate(0)

    if key == 27:
        camera.close()
        cv2.destroyAllWindows()
        break
