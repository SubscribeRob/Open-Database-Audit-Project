<?php
/**
 *
 * @namespace Audit\Cloud\Servers
 * @uri /jobs/{server_id}
 * @uri /jobs
 */
class JobsResource extends Resource {
     private $link;
     private $server_id;

     public function  __construct($server_id){
	$this->link = mysql_connect('localhost', 'root', 'Pn35x2dUW');
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
	$query = "SELECT * FROM jobs WHERE server_id IN (select server_id from servers where account_id = " . $AUTHENTICATED_ACCOUNT_ID . ")";

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

	global $AUTHENTICATED_USER_ID;

	if(trim($_POST['preprocessor']) == ''){
		$_POST['preprocessor'] = 'com.dbauditcloud.job.Policy';
	}

	$arr = explode('T',$_POST['end_date']);

	$_POST['end_date'] = $arr[0];

	$arr = explode('T',$_POST['start_date']);

	$_POST['start_date'] = $arr[0];

	if(isset($_POST['all_data']) != true || trim($_POST['all_data']) == ''){
		$_POST['all_data'] = 'false';
	}

	global $AUTHENTICATED_SERVER_ID;

	$query = sprintf("INSERT INTO jobs (server_id , pre_processor , post_processor ,tables, users, ips , ports, columns , create_sql , drop_sql, alter_sql, grant_sql , revoke_sql ,
  				           insert_sql , update_sql ,  delete_sql , select_sql , backup_sql , restore_sql , time_start , time_end ,  last_run_timestamp , last_run_name ,
  			     		   last_run_key_value_max , create_timestamp , alter_timestamp , cron,job_name, format, start_date,end_date,all_data )
				 VALUES (  %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s','%s',
					   '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', 
					   %d, '%s', NOW(), '%s', '%s' , '%s', '%s', '%s', %s)",
				  $AUTHENTICATED_SERVER_ID,mysql_real_escape_string($_POST['preprocessor']),mysql_real_escape_string('com.auditcloud.postprocessor'),mysql_real_escape_string($_POST['tables']),mysql_real_escape_string($_POST['users']),mysql_real_escape_string($_POST['ips']),mysql_real_escape_string($_POST['ports']),mysql_real_escape_string($_POST['columns']),mysql_real_escape_string($_POST['create_sql']),mysql_real_escape_string($_POST['drop_sql']),
				  mysql_real_escape_string($_POST['alter_sql']),mysql_real_escape_string($_POST['grant_sql']),mysql_real_escape_string($_POST['revoke_sql']),
				  mysql_real_escape_string($_POST['insert_sql']),mysql_real_escape_string($_POST['update_sql']),mysql_real_escape_string($_POST['delete_sql']),mysql_real_escape_string($_POST['select_sql']),mysql_real_escape_string($_POST['backup_sql']),mysql_real_escape_string($_POST['restore_sql']),mysql_real_escape_string($_POST['time_start']),mysql_real_escape_string($_POST['time_end']),mysql_real_escape_string($_POST['last_run_timestamp']),
				  mysql_real_escape_string($_POST['Last_run_name']),
				  mysql_real_escape_string($_POST['last_run_key_value_max']),mysql_real_escape_string($_POST['alter_timestamp']),mysql_real_escape_string($_POST['cron']), mysql_real_escape_string($_POST['policy_name']), mysql_real_escape_string($_POST['format']), mysql_real_escape_string($_POST['start_date']), mysql_real_escape_string($_POST['end_date']), mysql_real_escape_string($_POST['all_data']));


        $result = mysql_query($query);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}

	// Now get the ID of the job and insert the alerts
	$job_id = mysql_insert_id();

	global $AUTHENTICATED_USER_ID;

	if($_POST['preprocessor'] == 'com.dbauditcloud.job.DownloadServerData'){
			$query = sprintf("INSERT INTO notifications (job_id,type,title,body,user_id,acknowledged) VALUES (%d, '%s', '%s', '%s', %d, %d)",
					$job_id, 'E-Mail', 'Export ready for download for server ', 'Please click on the following link to download your data', $AUTHENTICATED_USER_ID, (($_POST['acknowledgement'] == false) ? 0 : 1));
			$result = mysql_query($query);
			if (!$result) {
			    $message  = 'Invalid query: ' . mysql_error() . "\n";
			    $message .= 'Whole query: ' . $query;
			    die($message);
			}

			global $AUTHENTICATED_USER_NAME;
			notify_panel(-1,'INFO','Download request for server  ' .  getServerNameByID($_POST['server_id']) . ' created by ' . $AUTHENTICATED_USER_NAME);
	}else if($_POST['prepocessor'] != 'com.dbauditcloud.job.DeleteData'){
		//Now insert the alerts
		foreach(explode(',',$_POST['users_alert']) as $alert){
			$query = sprintf("INSERT INTO notifications (job_id,type,title,body,user_id,acknowledged) VALUES (%d, '%s', '%s', '%s', %d, %d)",
					$job_id, 'E-Mail', 'Policy ' . $job_id . ' violation for server ', 'Please see web interface for more detail', $alert, (($_POST['acknowledgement'] == false) ? 0 : 1));
			$result = mysql_query($query);
			if (!$result) {
			    $message  = 'Invalid query: ' . mysql_error() . "\n";
			    $message .= 'Whole query: ' . $query;
			    die($message);
			}
			global $AUTHENTICATED_USER_NAME;
			notify_panel(-1,'INFO','New policy  ' .  mysql_real_escape_string($_POST['policy_name']) . ' created by ' . $AUTHENTICATED_USER_NAME);
		}
	}

	$this->refresh_job_scheduler();
	$response = new Response($request);
	$response->code = Response::OK;
        return $response;
        
    }

    function delete($request) {
	$tmp = json_decode($request->data);

	global $AUTHENTICATED_ACCOUNT_ID;
	$query = sprintf("SELECT *  FROM jobs WHERE id = %d AND server_id in (SELECT server_id FROM servers WHERE account_id = %d)", $tmp->id, $AUTHENTICATED_ACCOUNT_ID );
        $result = mysql_query($query);
	$row = mysql_fetch_assoc($result);

	if(!$row){
		die();
	}
        $query = sprintf("DELETE FROM jobs WHERE id = %d", $tmp->id );
        $result = mysql_query($query);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}

        $query = sprintf("DELETE FROM notifications WHERE job_id = %d", $tmp->id );
        $result = mysql_query($query);
        
	if (!$result) {
	    $message  = 'Invalid query: ' . mysql_error() . "\n";
	    $message .= 'Whole query: ' . $query;
	    die($message);
	}


	global $AUTHENTICATED_USER_NAME;
	notify_panel(-1,'INFO','Policy ' . $row['policy_name'] .' removed for server  ' .  getServerNameByID($row['server_id']) . ' by ' . $AUTHENTICATED_USER_NAME);

	$this->refresh_job_scheduler();
        $response = new Response($request);
        $response->code = Response::OK;
        return $response;
    }
    
    function refresh_job_scheduler(){
      $socket=socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
      socket_connect($socket, '127.0.0.1', '4444');
      socket_close($socket);
    }
}

