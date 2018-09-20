import numpy as np
import cv2

detector= cv2.CascadeClassifier('/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml')
cap = cv2.VideoCapture(0)

while(True):
    ret, img = cap.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    faces = detector.detectMultiScale(gray, 1.3, 5)
    
    height = np.size(img, 0)
    width = np.size(img, 1)
    count=0
    for (x,y,w,h) in faces:
	if count == 0:
        	cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2)
		print (str(height) +'x'+ str(width) + ' x: ' + str(x) + ' y: ' + str(y) + ' w: ' + str(w) + ' h:' + str(h))
		
		#definir si la captura esta cerca o lejos
		if (float(h)/float(height) < 0.5):
			print 'lejos ' + str(float(h)/float(height))
		elif (float(h)/float(height) > 0.5):
			print 'cerca ' + str(float(h)/float(height))

		#definir si la captura queda a la derecha, izquierda o centro de la camara.		
		if (x + w/2) < (width / 2):
			print 'izquierda'
		elif (x + w/2) > (width / 2):
			print 'derecha'


	count = count + 1	

    cv2.imshow('frame',img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
cap.release()
cv2.destroyAllWindows()

