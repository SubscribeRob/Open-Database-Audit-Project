<?php
/**
 *
 * @namespace Audit\Cloud\Servers
 * @uri /notifications/{server_id}
 * @uri /notifications
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
class NotificationsResource extends Resource {
     private $link;
     private $server_id;

     public function  __construct($server_id){
	$this->link = mysql_connect('localhost', $GLOBAL_DB_USERNAME, $GLOBAL_DB_PASSWORD);
	$this->server_id = $server_id['server_id']; 
	if (!$this->link) {
    		die('Could not connect: ' . mysql_error());
	}
	mysql_select_db('auditcloud'); 
     }


	public function __destruct(){
		mysql_close($this->link);
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
	

	$query = "SELECT * FROM notifications";

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


    function post($request) {

	
	$query = sprintf("INSERT INTO notifications notifications (job_id,type,title,body,user_id,acknowledged) values(%d,'%s','%s','%s',%d,%d)",
				   $_POST['job_id'],$_POST['type'],$_POST['title'],$_POST['body'],$_POST['user_id'],$_POST['acknowledged'] );


        $result = mysql_query($query);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}

	$response = new Response($request);
	$response->code = Response::OK;
        return $response;
        
    }

    function delete($request) {
        $query = sprintf("DELETE FROM notifications WHERE id = %d", $this->server_id);
        $result = mysql_query($query);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}
        $response = new Response($request);
        $response->code = Response::OK;
        return $response;
    }
    
}

