<?php
/**
 *
 * @namespace Audit\Cloud\Servers
 * @uri /users/{server_id}
 * @uri /users
 */

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


class UsersResource extends Resource {
     private $link;
     private $server_id;

     public function  __construct($server_id){
	$this->link = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
	$this->server_id = $server_id['server_id']; 
	if (!$this->link) {
    		die('Could not connect: ' . mysql_error());
	}
	mysql_select_db('joomla'); 
     }


	public function __destruct(){
		@mysql_close($this->link);
	}
    /**
     * Handle a GET request for this resource
     * @param Request request
     * @return Response
     */
    function get($request) {
        
        $response = new Response($request);

	    $ret['success'] = 'true';
	    $ret['message'] = 'Loaded data';
	

	global $AUTHENTICATED_ACCOUNT_ID;
	$query = "SELECT id,username user_name,email e_mail,account_id FROM cloud_users WHERE account_id = " . $AUTHENTICATED_ACCOUNT_ID;

	$i = 0;
	$result = mysql_query($query);
	while ($row = mysql_fetch_assoc($result)) {
		$ret['data'][$i++] = $row;
	}
	        
        

            $response->code = Response::OK;
            $response->addHeader('Content-type', 'text/plain');
            $response->body = json_encode($ret);
            
        
        return $response;
        
    }
	
    function error($test,$request){
   
	$response = new Response($request);
	$response->status = 500;
	$response->code = 500;
	echo $test;
	return $response;
     }

    function put($request){
	$_PUT  = array();  
    	if($_SERVER['REQUEST_METHOD'] == 'PUT') {  
       	 	parse_str($request->data, $_PUT);  
    	}
	if(strlen(trim($_PUT['user_name'])) < 6){
		return $this->error('The username must be atleast 6 characters long',$request);
	}
	if(strlen(trim($_PUT['password'])) < 6 && trim($_PUT['password']) != '12345'){
		return $this->error('The password must be atleast 6 characters long',$request);
	}
	if(!filter_var($_PUT['e_mail'], FILTER_VALIDATE_EMAIL)){
		return $this->error('The e-mail address is invalid',$request);
	}

	if(!$this->isValidUser($_POST['user_name'])){
		$this->error('The username already exists or is invalid',$request);
	}

	global $AUTHENTICATED_ACCOUNT_ID;

	$query = sprintf("UPDATE cloud_users SET username = '%s', email = '%s' %s WHERE id=%d AND ACCOUNT_ID = %d",
			 	mysql_real_escape_string(trim($_PUT['user_name'])), mysql_real_escape_string($_PUT['e_mail']),
				((trim($_PUT['password']) != '12345') ? ', password = \'' . md5($_PUT['password']) . '\'': ''),
				$_PUT['id'],$AUTHENTICATED_ACCOUNT_ID);
				
        $result = mysql_query($query);
	
	if (!$result) {
	    return $this->error('Unable to update into user: ' . mysql_error(),$request);
	}			 		

	global $AUTHENTICATED_USER_NAME;
	notify_panel(-1,'INFO','User ' . $_PUT['user_name'] . ' edited by ' . $AUTHENTICATED_USER_NAME);

	$response = new Response($request);
	$response->code = Response::OK;
        return $response;
    }


    function isValidUser($username){
		if(trim($username) == 'admin'){
			return false;
		}
	global $AUTHENTICATED_ACCOUNT_ID;

	//First check to see if the user exists in the database
	$query = sprintf("SELECT * FROM users WHERE user_name = '%s' AND account_id =%d",mysql_real_escape_string(trim($username)),$AUTHENTICATED_ACCOUNT_ID);


	$result = mysql_query($query);

	if($row = mysql_fetch_assoc($result)){
		return false;
	}

	return true;

    }
    function post($request) {
	
	if(strlen(trim($_POST['user_name'])) < 6){
		return $this->error('The username must be atleast 6 characters long',$request);
	}
	if(strlen(trim($_POST['password'])) < 6){
		return $this->error('The password must be atleast 6 characters long',$request);
	}

	if(!filter_var($_POST['e_mail'], FILTER_VALIDATE_EMAIL)){
		return $this->error('The e-mail address is invalid',$request);
	}

	if(!$this->isValidUser($_POST['user_name'])){
		$this->error('The username already exists or is invalid',$request);
	}

	global $AUTHENTICATED_ACCOUNT_ID;
	
	$query = sprintf("INSERT INTO cloud_users (name,username,password,email,account_id)  VALUES('%s','%s','%s','%s',%d)",  
		 mysql_real_escape_string(trim($_POST['user_name'])),mysql_real_escape_string(trim($_POST['user_name'])), mysql_real_escape_string(md5($_POST['password'])),
		 mysql_real_escape_string($_POST['e_mail']),$AUTHENTICATED_ACCOUNT_ID
		);

        $result = mysql_query($query);
 	if (!$result) {
	    return $this->error('Unable to insert into users: ' . mysql_error(),$request);
	}       

	$new_id = mysql_insert_id();
	
	$query = sprintf("INSERT INTO cloud_user_usergroup_map (user_id,group_id) values (%d,%d)",$new_id,2);
        $result = mysql_query($query);;
	if (!$result) {
	    return $this->error('Unable to insert into users: ' . mysql_error(),$request);
	}

	global $AUTHENTICATED_USER_NAME;

	notify_panel(-1,'INFO','New user ' . mysql_real_escape_string(trim($_POST['user_name'])) . ' created by ' . $AUTHENTICATED_USER_NAME);

	$response = new Response($request);
	$response->code = Response::OK;
        return $response;
        
    }

    function delete($request) {
	print_r($this);

	$username = getUsernameByID($this->server_id);


	global $AUTHENTICATED_ACCOUNT_ID;
        $query = sprintf("DELETE FROM cloud_users WHERE id = %d AND  account_id = %d", (int)$this->server_id,$AUTHENTICATED_ACCOUNT_ID);
        $result = mysql_query($query,$this->link);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}

	global $AUTHENTICATED_USER_NAME;
	notify_panel(-1,'INFO','User ' . $username . ' removed by ' . $AUTHENTICATED_USER_NAME);

        $response = new Response($request);
        $response->code = Response::OK;
        return $response;
    }

    
}

