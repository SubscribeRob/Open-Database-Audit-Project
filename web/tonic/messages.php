<?php
/**
 *
 * @namespace Audit\Cloud\Messages
 * @uri /messages/{server_id}
 * @uri /messages
 */
class MessageResource extends Resource {
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
	

	global $AUTHENTICATED_ACCOUNT_ID;
	$query = "SELECT id,message_type,message_timestamp,message_text  FROM messages WHERE account_id = ". mysql_real_escape_string($AUTHENTICATED_ACCOUNT_ID) . " ORDER BY id desc";
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

	
	$query = sprintf("INSERT INTO messages (server_id,message_type,message_timestamp,message_text)  VALUES(%d,'%s',NOW(),'%s')",  
		 mysql_real_escape_string($this->server_id), mysql_real_escape_string($_POST['message_type']),
		 mysql_real_escape_string($_POST['message_text'])
	 	);

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
        $query = sprintf("DELETE FROM messages WHERE id = %d", $_POST['id']);
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

