<?php
/* 
 *   This file is part of the Open Database Audit Project (ODAP).
 *
 *   ODAP is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Foobar is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   The code was developed by Rob Williams
 */

$user_id = null;
$link = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
if (!$link) {
    die('Could not connect: ' . mysql_error());
}

$db_selected = mysql_select_db('joomla', $link);
if (!$db_selected) {
    die ('Can\'t use foo : ' . mysql_error());
}

$query = sprintf("SELECT * FROM cloud_session,cloud_users  WHERE cloud_users.id = cloud_session.userid AND session_id='%s'",mysql_real_escape_string($_COOKIE['5789dd917c331561a0087968b29bcb27']));
$result = mysql_query($query);
if (!$result) {
    die('Invalid query: ' . mysql_error());
}else{
	if($row = mysql_fetch_assoc($result)){
		if(strlen($row['username']) < 1){
			header( 'Location: /index.php/component/users/?view=login' ) ;
			die();
		}
	}else{
		header( 'Location: /index.php/component/users/?view=login' ) ;
		die();
	}
}

$AUTHENTICATED_USER_ID = $row['userid'];
$AUTHENTICATED_USER_NAME = $row['username'];
$AUTHENTICATED_ACCOUNT_ID = $row['account_id'];


$db_selected = mysql_select_db('auditcloud', $link);
$query = "SELECT id,server_software FROM servers WHERE account_id = ". $AUTHENTICATED_ACCOUNT_ID;

$AUTHENTICATED_SERVERS = array();
$AUTHENTICATED_SERVERS_TYPES = array();

$result = mysql_query($query,$link);

	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}

$i =0;
while ($row = mysql_fetch_assoc($result)) {
	$AUTHENTICATED_SERVERS[$i] = $row['id'];
	$AUTHENTICATED_SERVERS_TYPES[$i++]  = $row['server_software'];
}


$AUTHENTICATED_SERVER_ID = -1;
$AUTHENTICATED_SERVERS_TYPE = -1;
if(isset($_GET['server_id']) == true || trim($_GET['server_id']) != ''){
	$key = -1;
	if(($key = array_search($_GET['server_id'],$AUTHENTICATED_SERVERS)) !== FALSE){
		$AUTHENTICATED_SERVER_ID =$_GET['server_id'];
		$AUTHENTICATED_SERVERS_TYPE = $AUTHENTICATED_SERVERS_TYPES[$key];
		if($AUTHENTICATED_SERVER_ID != $_GET['server_id']){
			die();
		}
	}
}else{
	if(isset($_POST['server_id']) == true || trim($_GET['server_id']) != ''){
		$key = -1;
		if(($key = array_search($_POST['server_id'],$AUTHENTICATED_SERVERS))  !== FALSE){
			$AUTHENTICATED_SERVER_ID =$_POST['server_id'];
			$AUTHENTICATED_SERVERS_TYPE = $AUTHENTICATED_SERVERS_TYPES[$key];
		}

		if($AUTHENTICATED_SERVER_ID != $_POST['server_id']){
			die();
		}
	}
	
}


mysql_close($link);

function notify_panel($server_id,$severity,$message){
	$link = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
	if (!$link) {
	    die('Could not connect: ' . mysql_error());
	}

	$db_selected = mysql_select_db('auditcloud', $link);
	if (!$db_selected) {
	    die ('Can\'t use foo : ' . mysql_error());
	}
		
	global $AUTHENTICATED_ACCOUNT_ID;
	$query = sprintf("INSERT INTO messages (server_id,account_id,message_type,message_timestamp,message_text) VALUES (%d,%d,'%s',%d,'%s')",
			 mysql_real_escape_string($server_id),mysql_real_escape_string($AUTHENTICATED_ACCOUNT_ID),mysql_real_escape_string($severity),time(),mysql_real_escape_string($message));

        $result = mysql_query($query);;
	if (!$result) {
	    die('Unable to getUsernameById: ' . mysql_error());
	}
}

function getUsernameByID($id){
	$link1 = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
	if (!$link1) {
	    die('Could not connect: ' . mysql_error());
	}

	$db_selected = mysql_select_db('joomla', $link1);
	if (!$db_selected) {
	    die ('Can\'t use foo : ' . mysql_error());
	}
	
	global $AUTHENTICATED_ACCOUNT_ID;
	$query = sprintf("SELECT username FROM cloud_users WHERE id = %d AND account_id = %d", $id,$AUTHENTICATED_ACCOUNT_ID);
	echo $query;
        $result = mysql_query($query);;
	if (!$result) {
	     die('Unable to getUsernameById: ' . mysql_error());
	}

	$row = mysql_fetch_assoc($result);

	return $row['username'];
}

function getServerNameByID($id){
	$link1 = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
	if (!$link1) {
	    die('Could not connect: ' . mysql_error());
	}

	$db_selected = mysql_select_db('auditcloud', $link1);
	if (!$db_selected) {
	    die ('Can\'t use foo : ' . mysql_error());
	}
	
	global $AUTHENTICATED_ACCOUNT_ID;
	$query = sprintf("SELECT server_name FROM servers WHERE id = %d AND account_id = %d", $id,$AUTHENTICATED_ACCOUNT_ID);
        $result = mysql_query($query);;
	if (!$result) {
	     die('Unable to getServerNameByID: ' . mysql_error());
	}

	$row = mysql_fetch_assoc($result);

	return $row['server_name'];
}
		
