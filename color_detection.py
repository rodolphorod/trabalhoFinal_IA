from __future__ import division
import cv2
import numpy as np
import time
import serial
from time import sleep


ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
ser.reset_input_buffer()
def nothing(*arg):
        pass

FRAME_WIDTH = 320
FRAME_HEIGHT = 240

# Initial HSV GUI slider values to load on program start.
#icol = (36, 202, 59, 71, 255, 255)    # Green
#icol = (18, 0, 196, 36, 255, 255)  # Yellow
#icol = (89, 0, 0, 125, 255, 255)  # Blue

#icol = (104, 117, 222, 121, 255, 255)   #  test
#icol = (0, 0, 0, 255, 255, 255)   # New start
cores  = {
    'vermelho':([0,20,90],[20,255,255]),
    'verde': ([36, 50, 59],[71, 255, 255]),
    'azul': ([0, 100, 70],[115, 255, 255])
    }
def selectColor(n):
    if(n==1): #RED
        lowTemp = [0,20,90]
        highTemp = [20,255,255]  
    if(n==2): #GREEN
        lowTemp  = [36, 50, 59]
        highTemp = [71, 255, 255]
    if(n==3):#YELLOW
        lowTemp = [18, 0, 196]
        highTemp = [36, 255, 255]
    if(n==4):
        lowTemp = [0, 100, 80]
        highTemp = [115, 255, 255]
    low = np.array(lowTemp)
    high = np.array(highTemp)  # Red)
    return low,high

# Initialize webcam. Webcam 0 or webcam 1 or ...
vidCapture = cv2.VideoCapture(0)
vidCapture.set(cv2.CAP_PROP_FRAME_WIDTH,FRAME_WIDTH)
vidCapture.set(cv2.CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT)
centroX=[0,0,0,0]
centroY=[0,0,0,0]
i=0
timeCheck = time.time()
while True:

    # Get webcam frame
    _, frame = vidCapture.read()
    #modificacao para commit
    # Show the original image.
    #cv2.imshow('frame', frame)
    # Convert the frame to HSV colour model.
    frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
#     print(frameHSV[159][119])
    
    # HSV values to define a colour range we want to create a mask from.
#     colorLow = np.array(lowRed)
#     colorHigh = np.array(highRed)
    aplica_np = lambda y:[np.array(x) for x in y]
    colorLow, colorHigh = aplica_np(cores['azul'])#selectColor(4)
#     colorLow = np.array(colorLow)
#     colorHigh = np.array(colorHigh)
    mask = cv2.inRange(frameHSV, colorLow, colorHigh)
    # Show the first mask
    cv2.rectangle(frame,(159,119),(160,120),(0,255,0),2)
    #cv2.imshow('mask-plain', mask)

    
    contours = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)[0]

    contour_sizes = [(cv2.contourArea(contour), contour) for contour in contours]
    if(len(contour_sizes)>1):
        biggest_contour = max(contour_sizes, key=lambda x: x[0])[1]
    
#     cv2.drawContours(frame, biggest_contour, -1, (0,255,0), 3)

        x,y,w,h = cv2.boundingRect(biggest_contour)
        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)
        centroX.append((int(x+w/2)))
        centroY.append((int(y+h/2)))
        pontoX = int((centroX[0]+2*centroX[1]+3*centroX[2]+4*centroX[3]+5*centroX[4])/15)
        pontoY = int((centroY[0]+2*centroY[1]+3*centroY[2]+4*centroY[3]+5*centroY[4])/15)
#             pontoY = int(sum(centroY)/len(centroY))
        saidaX = int((FRAME_WIDTH/2 - pontoX)/2)
        saidaY = int((FRAME_HEIGHT/2-pontoY)/2)
        #print(F' {len(centroX)} ({saidaX},{saidaY})')
        centroX.pop(0)
        centroY.pop(0)
        cv2.rectangle(frame,(pontoX,pontoY),(pontoX+1,pontoY+1),(0,255,0),2)
        if(i==3):
            i=0
        i=i+1
        if(time.time()-timeCheck >0.5):
            ser.reset_input_buffer()
            c = f'{saidaX} {saidaY}'

            ser.write(c.encode('utf-8'))
            print (c)
            ser.write('\n'.encode('utf-8'))
            timeCheck = time.time()
            print(timeCheck)
    
    
    # Show final output image
    
    #print(f'T:{c}\n');

    #sleep(1)
#     if ser.in_waiting > 0:	
#             line = ser.readline().decode('utf-8').rstrip()
#             print(line)
    #sleep(1)
    #cv2.imshow('colorTest', frame)
    
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break
#     print(F'fps - {1/(time.time() - timeCheck)}')
    
cv2.destroyAllWindows()
vidCapture.release()