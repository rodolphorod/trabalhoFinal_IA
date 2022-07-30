from __future__ import division
import cv2
import numpy as np
import time

def nothing(*arg):
        pass

FRAME_WIDTH = 320
FRAME_HEIGHT = 240

# Initial HSV GUI slider values to load on program start.
#icol = (36, 202, 59, 71, 255, 255)    # Green
#icol = (18, 0, 196, 36, 255, 255)  # Yellow
#icol = (89, 0, 0, 125, 255, 255)  # Blue
lowRed = [0,50,60]
highRed = [10,255,255]  # Red
#icol = (104, 117, 222, 121, 255, 255)   #  test
#icol = (0, 0, 0, 255, 255, 255)   # New start

# Initialize webcam. Webcam 0 or webcam 1 or ...
vidCapture = cv2.VideoCapture(0)
vidCapture.set(cv2.CAP_PROP_FRAME_WIDTH,FRAME_WIDTH)
vidCapture.set(cv2.CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT)
centroX=[0,0,0]
centroY=[0,0,0]
i=0
while True:
    timeCheck = time.time()

    # Get webcam frame
    _, frame = vidCapture.read()

    # Show the original image.
    cv2.imshow('frame', frame)
    # Convert the frame to HSV colour model.
    frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # HSV values to define a colour range we want to create a mask from.
    colorLow = np.array(lowRed)
    colorHigh = np.array(highRed)
    mask = cv2.inRange(frameHSV, colorLow, colorHigh)
    # Show the first mask
    cv2.imshow('mask-plain', mask)

    
    contours = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)[0]

    contour_sizes = [(cv2.contourArea(contour), contour) for contour in contours]
    if(len(contour_sizes)>1):
        biggest_contour = max(contour_sizes, key=lambda x: x[0])[1]
    
#     cv2.drawContours(frame, biggest_contour, -1, (0,255,0), 3)

        x,y,w,h = cv2.boundingRect(biggest_contour)
        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)
        centroX.append((int(x+w/2)))
        centroY.append((int(y+h/2)))
        pontoX = int((centroX[0]+2*centroX[1]+3*centroX[2]+5*centroX[3])/11)
        pontoY = int((centroY[0]+2*centroY[1]+3*centroY[2]+5*centroY[3])/11)
#             pontoY = int(sum(centroY)/len(centroY))
        print(F' {len(centroX)} ({FRAME_WIDTH/2 - pontoX},{FRAME_HEIGHT/2-pontoY})')
        centroX.pop(0)
        centroY.pop(0)
        cv2.rectangle(frame,(pontoX,pontoY),(pontoX+1,pontoY+1),(0,255,0),2)
        if(i==3):
            i=0
        i=i+1
            
            
        
    
#     cv2.drawContours(frame, contours, -1, (0,255,0), 3)
#     
#     cv2.drawContours(frame, contours, 3, (0,255,0), 3)
        
#         cnt = contours[1]
#         cv2.drawContours(frame, [cnt], 0, (0,255,0), 3)
    
    # Show final output image 
    cv2.imshow('colorTest', frame)
	
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break
#     print(F'fps - {1/(time.time() - timeCheck)}')
    
cv2.destroyAllWindows()
vidCapture.release()