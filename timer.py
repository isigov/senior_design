#!/usr/bin/python
import os
import sys
import time
import MySQLdb

def main():
        db = MySQLdb.connect(host="localhost",
                     user="root",
                     passwd="googleme123",
                     db="authenticator")
        cur = db.cursor()
        user = raw_input("Enter username to check authentication status: ")
        cur.execute("SELECT * FROM users")  
        for row in cur.fetchall():
                if user in str(row[1]):
                        if int(time.time()) - int(row[2]) > 600: #10 minutes
                                print "Not authenticated"
                        else:
                                print "Authenticated"
if __name__ == '__main__':
        main()