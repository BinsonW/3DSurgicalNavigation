# Image process thread
import cv2
from PyQt5 import QtWidgets, QtCore
import globalvar as gl
import numpy as np
# Processframe thread


class processimg(QtCore.QObject):
    signal_finish = QtCore.pyqtSignal()

    def __init__(self):
        super(processimg, self).__init__()

        # Setup SimpleBlobDetector parameters.
        self.params = cv2.SimpleBlobDetector_Params()

        # Change thresholds
        self.params.minThreshold = 10
        self.params.maxThreshold = 200

        # Filter by Area.
        self.params.filterByArea = False
        self.params.minArea = 1500

        # Filter by Circularity
        self.params.filterByCircularity = False
        self.params.minCircularity = 0.1

        # Filter by Convexity
        self.params.filterByConvexity = False
        self.params.minConvexity = 0.87

        # Filter by Inertia
        self.params.filterByInertia = False
        self.params.minInertiaRatio = 0.01

        # Create a detector with the parameters
        self.detector = cv2.SimpleBlobDetector_create(self.params)
        self.i = 0

    def findmarker(self):
        # ptvsd.debug_this_thread()
        gl._semacq(1)
        # Detect blobs.
        self.keypoints = self.detector.detect(gl.frame[self.i])

        # Draw detected blobs as red circles.
        # cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures
        # the size of the circle corresponds to the size of blob

        gl.frame[self.i] = cv2.drawKeypoints(gl.frame[self.i], self.keypoints, np.array(
            []), (0, 0, 255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
        gl._semrel(2)
        self.signal_finish.emit()
        if self.i == 9:
            self.i = 0
        else:
            self.i += 1

    def solvePnP(self):
        pass
