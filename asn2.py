#!/usr/bin/python
from libmich.asn1.processor import *;
import os
import sys
import time

def decodePCCH(pcchHex): 
	pcch = GLOBAL.TYPE['PCCH-Message']
	buf = pcchHex.decode('hex')
	pcch.decode(buf)
	fstr = pcch()
	try:
		for i in fstr['message'][1][1]['pagingRecordList']:
			num = str(i['ue-Identity'][1]['m-TMSI'][0])
			#print num
			if "12582912" in num:
				print "Found Illya"
				print num
			
			#print "%s -- %s" % (i['ue-Identity'][1]['m-TMSI'][0], pcchHex)
	except:
		print fstr
	#show(pcch)

load_module('RRCLTE')
ASN1.ASN1Obj.CODEC = PER
PER.VARIANT= 'U'

while 1:
	time.sleep(0.05)
	try:
		pipe = os.open("/tmp/pdsch_ue", os.O_RDONLY | os.O_NONBLOCK)
		reader = os.fdopen(pipe)
		line = reader.readline()
	except:
		continue
	if line:
		if line.startswith("["):
			line = line[1:-4] #Stripping line 
			line = line.replace(" ", "")
			decodePCCH(line)
	
 
os.close(pipe);
