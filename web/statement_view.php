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

$GLOBALS['THRIFT_ROOT'] = 'thrift';
require_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';
require_once $GLOBALS['THRIFT_ROOT'].'/protocol/TBinaryProtocol.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TSocket.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TBufferedTransport.php';
require_once $GLOBALS['THRIFT_ROOT'].'/packages/UIHbase/DisplayEvents.php';

    // Create a thrift connection (Boiler plate)
$socket = new TSocket('localhost', '9091');
$socket->setRecvTimeout(10000);
$transport = new TBufferedTransport($socket);
$protocol = new TBinaryProtocol($transport);

$client = new DisplayEventsClient($protocol);
$transport->open();

$events = $client->getEventsByRowID($AUTHENTICATED_SERVER_ID, $_GET['row_id'],$AUTHENTICATED_SERVERS_TYPE);

$transport->close();

require_once 'XML/Serializer.php';

// An array of serializer options
$serializer_options = array (  
    'addDecl' => TRUE, 
    'encoding' => 'ISO-8859-1',
    'indent' => '  ', 
    'rootName' => 'events' ,
    'defaultTagName' => 'event'
); 
$Serializer = new XML_Serializer($serializer_options);

$status = $Serializer->serialize($events); 
if (PEAR::isError($status)) { 
    die($status->getMessage());
}
header('Content-type: text/xml');
echo $Serializer->getSerializedData();
?>
