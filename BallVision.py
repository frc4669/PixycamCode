import threading
from networktables import NetworkTables
from pixy import *
from ctypes import *
import math

cond = threading.Condition()
notified = [False]

def connectionListener(connected, info):
    print(info, '; Connected=%s' % connected)
    with cond:
        notified[0] = True
        cond.notify()

NetworkTables.initialize(server='10.46.69.2')
NetworkTables.addConnectionListener(connectionListener, immediateNotify=True)

with cond:
    print("Waiting")
    if not notified[0]:
        cond.wait()
print("Connected!")
        
# Insert your processing code here

table = NetworkTables.getTable("DataTable")

print ("Pixy Python SWIG Example -- Get blocks")

# Initialize Pixy Interpreter thread #
pixy_init()

class blocks (Structure):
  _fields_ = [ ("type", c_uint),
               ("signature", c_uint),
               ("x", c_uint),
               ("y", c_uint),
               ("width", c_uint),
               ("height", c_uint),
               ("angle", c_uint) ]

blocks = BlockArray(100)

#Pixels
PIXY_IMAGE_WIDTH = 320
PIXY_IMAGE_HEIGHT = 200
#Inches
BALL_DIAMETER = 13
#Degrees
PIXY_FOV_ANGLE = 75

# Wait for blocks #
while 1:

  orangeIndex = -1
  maxAreaOrange = 0
  count = pixy_get_blocks(100, blocks)

  if count > 0:
    # blocks found #
	for index in range (0, count):
		currentArea = blocks[index].width*blocks[index].height
		if currentArea > 50:
			if blocks[index].signature==1:
				maxAreaOrange = max(currentArea, maxAreaOrange)
				if maxAreaOrange==currentArea:
					orangeIndex = index
	if orangeIndex != -1:
		for index in range (0, count):
			table.putNumber("orangeX", blocks[orangeIndex].x)
			table.putNumber("orangeY", blocks[orangeIndex].y)
			table.putNumber("orangeWidth", blocks[orangeIndex].width)
			table.putNumber("orangeHeight", blocks[orangeIndex].height)
                        focalLength = PIXY_IMAGE_WIDTH/(2*math.tan(math.radians(PIXY_FOV_ANGLE/2)))
                        horizontalAngle = math.degrees(math.atan2((blocks[orangeIndex].x - PIXY_IMAGE_WIDTH/2), focalLength))
                        distance = BALL_DIAMETER * PIXY_IMAGE_WIDTH/ (2 * blocks[orangeIndex].width * math.tan(math.radians(PIXY_FOV_ANGLE / 2)));
      			print '[blocks_TYPE=%d SIG=%d X=%3d Y=%3d WIDTH=%3d HEIGHT=%3d HORANGLE=%3f DISTANCE=%3f]' % (blocks[orangeIndex].type, blocks[orangeIndex].signature, blocks[orangeIndex].x, blocks[orangeIndex].y, blocks[orangeIndex].width, blocks[orangeIndex].height, horizontalAngle, distance)
      			

