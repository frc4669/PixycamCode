import threading
from networktables import NetworkTables
from pixy import *
from ctypes import *

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
        
# Insert your processing code here
print("Connected!")

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
      			print '[blocks_TYPE=%d SIG=%d X=%3d Y=%3d WIDTH=%3d HEIGHT=%3d]' % (blocks[orangeIndex].type, blocks[orangeIndex].signature, blocks[orangeIndex].x, blocks[orangeIndex].y, blocks[orangeIndex].width, blocks[orangeIndex].height)
