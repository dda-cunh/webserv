#!/usr/bin/php

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
	$userInput = escapeshellarg($_POST['args']);

	$output = shell_exec("cowsay $userInput");

	echo "<pre>$output</pre>";
}

?>
