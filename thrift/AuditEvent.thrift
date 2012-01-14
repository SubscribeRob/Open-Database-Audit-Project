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

namespace java com.odap.common.thrift

struct Event {
        1: string src_ip,
        2: i32 src_port,
        3: i32 timestamp,
        5: string statement,
	6: string dbname,
	7: string appname,
	8: string username,
	9: i32 row_count
}

struct Message {
        1: i16 server_id,
        2: string token,
        3: list<Event> events
}

struct ConfigMessage{
       1: string username,
       2: string password,
       3: string server_software,
       4: i16 port,
       5: string server_name,
       6: double timezone_offset,
       7: bool strip_predicates,
       8: string token,
       9: i16 server_id
      10: string server
}

struct DiscoveryObject {
	1: i32	discovery_id,
	2: string operator,
	3: string name,
	4: string value,
	5: double percentage,
	6: string category
}

struct SensitiveObject{
	1: string table_name,
	2: string dbname,
	3: string discovery_id
}

service OdapService{
	void store(1: Message msg)
	void heartbeat(1: i16 server_id, 2: string token)

	ConfigMessage registerNewServer(1: ConfigMessage config)
	ConfigMessage getConfig(1: ConfigMessage config)

	list<DiscoveryObject> getDiscoveryObjects(1: i16 server_id, 2: string token)
	void setSensitiveObjects(1: i16 server_id, 2: string token, 3: list<SensitiveObject> sensitive_objects)

}
