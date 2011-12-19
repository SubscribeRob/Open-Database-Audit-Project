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

Ext.define('PolicyAppGridPanel', {
	extend: 'Ext.grid.Panel',

	alias:  'policy.gridpanel',

	
    initComponent: function(){
        this.addEvents(
            'rowdblclick', //Fires when a row is double clicked
            'select'		
        );


        Ext.apply(this, {
            cls: 'feed-grid',
            store: Ext.create('Ext.data.Store', {
				pageSize: 50,
				model: 'Policy.Model',
				remoteSort: true,
				autoLoad: true,
				autoSync: true,
				proxy: {
				    // load using script tags for cross domain, if the data in on the same domain as
				    // this page, an HttpProxy would be better
				    type: 'rest',
				    url: 'tonic/jobs/1',
				    		reader: {
							type: 'json',
							root: 'data'
				    		},
			 		        writer: {
							type: 'json'
						}
				},
                listeners: {
                    load: this.onLoad,
                    scope: this
                }
            }),
            viewConfig: {
                itemId: 'view',
                listeners: {
                    scope: this,
                    itemdblclick: this.onRowDblClick
                }
            },
            columns: [{
                text: 'Server ID',
                dataIndex: 'server_id',
                width: 55

            }, {
                text: 'Policy Name',
                dataIndex: 'job_name',
                width: 175

            }, {
                text: 'Outstanding Violations',
                dataIndex: 'violations_not_acknowledged',
                width: 120
            }, {
                text: 'Violations',
                dataIndex: 'violations_acknowledged',
		align: 'center',
                width: 123
            }, {
                text: 'Total Violations',
                dataIndex: 'violations_total',
		align: 'center'
            }, {
                text: 'Statements Processed',
                dataIndex: 'total_statements_analyzed',
		align: 'center',
                width: 120
            }, {
                text: 'User CPU',
                dataIndex: 'total_cpu_user',
		align: 'center',
                width: 70
            }, {
                text: 'System CPU',
                dataIndex: 'total_cpu_system',
		align: 'center',
                width: 70
            }]
        });
        this.callParent(arguments);
        this.on('selectionchange', this.onSelect, this);
    },

        /**
     * Reacts to a double click
     * @private
     * @param {Object} view The view
     * @param {Object} index The row index
     */
    onRowDblClick: function(view, record, item, index, e) {
        this.fireEvent('rowdblclick', this, this.store.getAt(index));
    },


    /**
     * React to a grid item being selected
     * @private
     * @param {Ext.model.Selection} model The selection model
     * @param {Array} selections An array of selections
     */
    onSelect: function(model, selections){
        var selected = selections[0];
        if (selected) {
            this.fireEvent('select', this, selected);
        }
    },

    /**
     * Listens for the store loading
     * @private
     */
    onLoad: function(){
        //this.getSelectionModel().select(0);
    },

    /**
     * Instructs the grid to load a new feed
     * @param {String} url The url to load
     */
    loadPolicies: function(url){
        var store = this.store;
        store.load();
    }
});
