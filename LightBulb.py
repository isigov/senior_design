import colorsys
import pexpect

# Dependencies:
# - You must install the pexpect library, typically with 'sudo pip install pexpect'.
# - You must have bluez installed and gatttool in your path (copy it from the
#   attrib directory after building bluez into the /usr/bin/ location).


def Lightbulb(address, authenticated):

	#address is MAC address of lightbulb
	#expected format: 5C:31:3E:F2:16:13
	#boolean value for authenticated

	# Run gatttool interactively.
	gatt = pexpect.spawn('gatttool -I')

	# Connect to the device.
	gatt.sendline('connect {0}'.format(bulb))
	gatt.expect('Connection successful')


	if authenticated == True:
		#power on
		gatt.sendline('char-write-cmd 0x0028 58010301ffffffffff')
	else:
		#power off
		gatt.sendline('char-write-cmd 0x0028 58010301ff00000000')




    
