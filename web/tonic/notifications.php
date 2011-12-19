<?php
/**
 *
 * @namespace Audit\Cloud\Servers
 * @uri /notifications/{server_id}
 * @uri /notifications
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

