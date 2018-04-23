<?php

//echo $_POST["username"] . "\n" . $_POST["password"];

$mysqli = new mysqli("localhost", "root", "googleme123", "authenticator");

$checkUser = $mysqli->prepare('SELECT * FROM logins WHERE username = ?');
$checkUser->bind_param('s', $_POST["username"]);

$checkUser->execute();

$result = $checkUser->get_result();

#Calculate hash of entered password and compare with the values in the database
while ($row = $result->fetch_assoc()) {
	if(strpos($row['passhash'], md5($_POST["password"])) !== false)
	{
		$stmt = $mysqli->prepare('SELECT * FROM users WHERE id = ?');
		$stmt->bind_param('i', $row['id']);

		$stmt->execute();

		$result = $stmt->get_result();
		while ($row = $result->fetch_assoc()) {
			$time = time();
			#Easily integrate other frameworks just by adding them to this file
			#Check if current time is < 10 minutes from last authenticated time
			if($_POST["action"] == "on" && (int)$row['last_login'] - $time <= 600)
			{
				#Execute script to turn lightbulb on
				echo shell_exec("python /home/iota/senior_design/iot.py True");
			}
			if($_POST["action"] == "off" && (int)$row['last_login'] - $time <= 600)
            {       
            	#Execute script to turn lightbulb off
                echo shell_exec("python /home/iota/senior_design/iot.py False");
            }
			break;
		}
		break;
	}
}

?>