import sys
from PyQt5 import QtWidgets, QtCore
import cv2 as cv
import numpy as np
def _init():
    # Semaphore
    # sem0 acquire img from camera, release sem1
    # sem1 show initial img, release sem2
    # sem2 process img, release sem3
    # sem3 show processed img and solvePNP, release sem0 and refresh 3dview's camera 
    frameBufferSize=10
    semnum=4
    global sem
    sem=[]
    for i in range(semnum):
        sem.append(QtCore.QSemaphore(frameBufferSize))

    # frame list
    
    global frame,frame_proc,width,height
    width=480
    height=640
    frame=np.empty((frameBufferSize,width,height,3),np.uint8)
    frame_proc=np.empty((frameBufferSize,width,height,3),np.uint8)
    # marker list

def _semrel(i):
    global sem
    sem[i].release()
def _semacq(i):
    global sem
    sem[i].acquire()
