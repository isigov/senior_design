# LTE Authenticator Code Documentation

## *./senior_design/add_user.py*
This script provides the user with an ability to isolate the TMSI of a device and automatically create the necessary database entries for a new user. Currently, this script only supports T-Mobile’s band 4, but this can be changed on line 101 to reflect the frequency the user is current using. This script uses a named pipe to read information passed by the pdsch_ue application, but this method could be improved by directly integrating all functionality into the main application. The default MySQL database information on line 148 and the Gmail login on line 49 should both be updated to reflect the correct configuration.

## *./senior_design/asn2.py*
This script uses a named pipe to read information passed by the pdsch_ue application and compare it against the database. The default MySQL database information on line 31 should be updated to reflect the correct configuration. Any other modes of authentication should follow the *decodePCCH* function model to change a user's authenticated state.

## *./senior_design/iot.py*
This script takes a boolean as a parameter and turns our lightbulb on/off. To add different varieties of IOT devices, this script should be modified to include respective SDKs or functionality.

## *./senior_design/iot.php*
This script handles requests by the application to turn our lightbulb on/off. It checks that the provided login is correct and executes the iot.py script.

## *./senior_design/info.php*
This script handles login requests by the application and updates the last authenticated time of a user. 

## *./senior_design/srsLTE/lib/examples/pdsch_ue.c*
This is the main application interacting with the USRP that captures paging messages and forwards them to named pipe to be read by the *asn2.py* script. To speed up our system, the RRC decoding library should be implemented in this script instead of a separate Python script. This would give the system the ability to immediately discard irrelevant packets.
