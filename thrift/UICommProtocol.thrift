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

struct DisplayEvent {
	1: string row_id,
	2: i32 timestamp,
        3: string src_ip,
        4: string src_port,
	5: string dbname,
	6: string appname,
	7: string username,
	8: string statement,
	9: string freqency,
	10: string security_risk,
	11: string tables,
	12: string columns,
	13: string similar_statements
}

service DisplayEvents {
        list<DisplayEvent> getEventsByRowID(1:i16 server_id, 2:string row_id,3:string database_type)
}
