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

$feed = $_REQUEST['feed'];
if($feed != '' && strpos($feed, 'http') === 0){
	header('Content-Type: text/xml');
	$xml = file_get_contents($feed);
	$xml = str_replace('<content:encoded>', '<content>', $xml);
	$xml = str_replace('</content:encoded>', '</content>', $xml);
	$xml = str_replace('</dc:creator>', '</author>', $xml);
	echo str_replace('<dc:creator', '<author', $xml);
	return;
}
