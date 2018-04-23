import os
import sys
import tempfile
import argparse
import subprocess
import time
import datetime
import hashlib
from shutil import copyfile
from libmich.asn1.processor import *;
import MySQLdb
import smtplib
from threading import Thread
import sys
import signal

gutiList = []
currentList = []

#Default phone/provider

phone = "6177752242"
provider_domain = "tmomail.net"
iter_count = 1

#Continuously read from pipe, format the line, and decode the structure
def parse():
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

#Send an email via our gmail account
def sendMail(recipient, content):
    import smtplib
    from email.MIMEMultipart import MIMEMultipart
    from email.MIMEText import MIMEText

    gmailUser = 'teamiotabu@gmail.com'
    gmailPassword = 'iotarulez'
    message=content

    msg = MIMEMultipart()
    msg['From'] = gmailUser
    msg['To'] = recipient
    msg['Subject'] = "testing"
    msg.attach(MIMEText(message))

    mailServer = smtplib.SMTP('smtp.gmail.com', 587)
    mailServer.ehlo()
    mailServer.starttls()
    mailServer.ehlo()
    mailServer.login(gmailUser, gmailPassword)
    mailServer.sendmail(gmailUser, recipient, msg.as_string())
    mailServer.close()

#Look for TMSI in the packet, add it to the current list
def decodePCCH(pcchHex): 
	pcch = GLOBAL.TYPE['PCCH-Message']
	buf = pcchHex.decode('hex')
	pcch.decode(buf)
	fstr = pcch()
	try:
		for i in fstr['message'][1][1]['pagingRecordList']:
			num = str(i['ue-Identity'][1]['m-TMSI'][0])
			currentList.append(num)
	except:
		pass


def main():
	global currentList
	global gutiList
	global phone
	global provider_domain
	global iter_count

	if len(sys.argv) != 4:
		print "Usage: " + sys.argv[0] + " [phone number] [provider] [iteration]"
		return

	phone = sys.argv[1]
	provider_domain = sys.argv[2]
	iter_count = int(sys.argv[3])

	load_module('RRCLTE')
	ASN1.ASN1Obj.CODEC = PER
	PER.VARIANT= 'U'

	#Parameters for pdsch_ue application, -r 0xfffe filters paging packets, 2140e6 is the downlink frequency of T-Mobile's band 4, -l 1 chooses the closest tower
	asnc_options = {'-r': '0xfffe', '-f': '2140e6', '-l': '1'}
	#Hardcoded path for the application
	command = [os.path.expanduser('~/senior_design/srsLTE/build/lib/examples/pdsch_ue')]
	for key, value in asnc_options.iteritems():
		command.extend((key, str(value)))
	DEVNULL = open(os.devnull, 'wb')
	#Execute with root privileges
	p = subprocess.Popen('echo {} | sudo -S {}'.format("iotarulez", " ".join(command)), stdout=DEVNULL, stderr=DEVNULL, shell=True)

	#Start parse thread, so main thread is not locked
	thread = Thread(target = parse)
    thread.start()

	time.sleep(15)

	#Send 5 SMS/emails per round 
	matches_list = set()
	for i in range(0, iter_count):
		print "Round " + str(i + 1) + " of emails."
		gutiList.append([])
		currentList = gutiList[i]
		sendMail(phone + "@" + provider_domain, str(time.time()))
		time.sleep(4)
		sendMail(phone + "@" + provider_domain, str(time.time()))
		time.sleep(4)
		sendMail(phone + "@" + provider_domain, str(time.time()))
		time.sleep(4)
		sendMail(phone + "@" + provider_domain, str(time.time()))
		time.sleep(4)
		sendMail(phone + "@" + provider_domain, str(time.time()))
		time.sleep(45)
		#See which TMSI numbers show up across multiple runs
		if i - 1 < 0:
			matches_list = set(gutiList[i])
		else:
			matches_list = matches_list.intersection(gutiList[i])

	#Print the output and have the user choose which TMSI (if any) to add to the database
	for i in range(0, len(matches_list)):
		print "{0}: {1}".format(i, matches_list[i])

	num = input("Type the number of a GUTI to add it to the database\n")
	device = raw_input("Enter a device name to associate with this GUTI\n")
	user = raw_input("Enter a user name to associate with this GUTI\n")
	passw = raw_input("Enter a password to associate with this GUTI\n")

	#Insert the data into the local database
	db = MySQLdb.connect(host="localhost", user="root", passwd="googleme123",db="authenticator")
	db.autocommit(True)
	cur = db.cursor()
	cur.execute("INSERT INTO users VALUES ('%d', '%s', '%d')" % (int(gutiList[num]), device,0))

	m = hashlib.md5()
	m.update(passw)
	cur.execute("INSERT INTO logins VALUES ('%d', '%s', '%d')" % (int(gutiList[num]), user, m.hexdigest()))

	#Kill the pdsch_ue process
	os.killpg(os.getpgid(p.pid), signal.SIGTERM)
	
	sys.exit()




if __name__ == '__main__':
	main()