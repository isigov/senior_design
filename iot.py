
import colorsys
import math
import sys
import time

import pexpect


# Get Auth status from command parameters.
if len(sys.argv) != 2:
    print 'Error must specify authorized status as parameter!'
    sys.exit(1)
status = sys.argv[1]

# Run gatttool interactively.
gatt = pexpect.spawn('gatttool -I')

# Connect to the device.
gatt.sendline('connect B4:99:4C:64:84:A6')
gatt.expect('Connection successful')
print('Connection successful')


# Enter main loop.
if status == 'True' or status == 'true':
	while True:
		print('power on')
		gatt.sendline('char-write-cmd 0x0028 58010301ff00ffffff')
		gatt.sendline('char-write-cmd 0x0028 58010301ff00ffffff')
		break

elif status == 'False' or status == 'false':
	while True:
		print('power off')
		gatt.sendline('char-write-cmd 0x0028 58010301ff00000000')
		gatt.sendline('char-write-cmd 0x0028 58010301ff00000000')
		break



