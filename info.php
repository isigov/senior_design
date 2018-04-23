<?php

//echo $_POST["username"] . "\n" . $_POST["password"];

#Update last authenticated time if credentials match

$mysqli = new mysqli("localhost", "root", "googleme123", "authenticator");

$checkUser = $mysqli->prepare('SELECT * FROM logins WHERE username = ?');
$checkUser->bind_param('s', $_POST["username"]);

$checkUser->execute();

$result = $checkUser->get_result();

while ($row = $result->fetch_assoc()) {
	if(strpos($row['passhash'], md5($_POST["password"])) !== false)
	{
		$stmt = $mysqli->prepare('SELECT * FROM users WHERE id = ?');
		$stmt->bind_param('i', $row['id']);

		$stmt->execute();

		$result = $stmt->get_result();
		
		$time = 0;
		if($_POST["action"] == "auth")
		{
			$time = time();
		}
		while ($row = $result->fetch_assoc()) {
			$q = "UPDATE users SET last_login = " . $time . " WHERE id = " . $row['id'];
			mysqli_query($mysqli, $q);
		}
		echo "Successful login!";
		break;
	}
}

?>