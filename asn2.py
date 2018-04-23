from libmich.asn1.processor import *;
import os
import sys
import time
import MySQLdb

guti = []
username = []

##Look for TMSI in the packet
def decodePCCH(pcchHex, cursor): 
        pcch = GLOBAL.TYPE['PCCH-Message']
        buf = pcchHex.decode('hex')
        pcch.decode(buf)
        fstr = pcch()
        try:
                for i in fstr['message'][1][1]['pagingRecordList']:
                        num = str(i['ue-Identity'][1]['m-TMSI'][0])
                        if int(num) in guti:
                                idx = guti.index(int(num))
                                #If a matching TMSI is found, update the last authenticated time
                                print "Found %s, updating time..." % username[idx]
                                cursor.execute("UPDATE users SET last_login = '%d' WHERE id = '%d'" % (int(time.time()), int(num)))
        except:
                pass

load_module('RRCLTE')
ASN1.ASN1Obj.CODEC = PER
PER.VARIANT= 'U'

db = MySQLdb.connect(host="localhost",
                     user="root",
                     passwd="googleme123",
                     db="authenticator")
db.autocommit(True)
cur = db.cursor()
cur.execute("SELECT * FROM users")  
for row in cur.fetchall() :
        guti.append(int(row[0]))
        username.append(str(row[1]))

#Continuously read from pipe, format the line, and decode the structure
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
                        decodePCCH(line, cur)
os.close(pipe);