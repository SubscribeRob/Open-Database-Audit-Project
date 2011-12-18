/*

This file is part of Ext JS 4

Copyright (c) 2011 Sencha Inc

Contact:  http://www.sencha.com/contact

GNU General Public License Usage
This file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE included in the packaging of this file.  Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.

If you are unsure which license is appropriate for your use, please contact the sales department at http://www.sencha.com/contact.

*/

Ext.define('FeedEvent', {
    extend: 'Ext.data.Model',
    fields: ['row_id', 'src_ip', 'src_port', {name:'timestamp',type:'date', dateFormat:'timestamp'}, 'dbname', 'appname','username','statement','freqency','security_risk','tables','columns','similar_statements']
});

Ext.Loader.setConfig({enabled:true});
        Ext.Loader.setPath('Ext.ux', 'ux/');
        Ext.require([
            'Ext.ux.LiveSearchGridPanel'
        ]);

Ext.define('FeedViewer.FeedGrid', {
    extend: 'Ext.ux.LiveSearchGridPanel',

    alias: 'widget.feedgrid',

    initComponent: function(){
        this.addEvents(
            /**
             * @event rowdblclick
             * Fires when a row is double clicked
             * @param {FeedViewer.FeedGrid} this
             * @param {Ext.data.Model} model
             */
            'rowdblclick',
            /**
             * @event select
             * Fires when a grid row is selected
             * @param {FeedViewer.FeedGrid} this
             * @param {Ext.data.Model} model
             */
            'select'
        );

	this.max_row_id = '-1';

        Ext.apply(this, {
            cls: 'feed-grid',
            store: Ext.create('Ext.data.Store', {
                model: 'FeedEvent',
                proxy: {
                    type: 'ajax',
                    url: 'statement_view.php?server_id=' + this.server_id + '&row_id=' + this.max_row_id,
                    reader: {
                        type: 'xml',
                        record: 'event'
                    }
                },
                listeners: {
                    load: this.onLoad,
                    scope: this
                }
            }),
            viewConfig: {
                itemId: 'view',
                plugins: [{
                    pluginId: 'preview',
                    ptype: 'preview',
                    bodyField: 'description',
                    expanded: true
                }],
                listeners: {
                    scope: this,
                    itemdblclick: this.onRowDblClick
                }
            },
            columns: [{
                text: 'Statements',
                dataIndex: 'statement',
                flex: 1,
                renderer: this.formatTitle
            }, {
                text: 'Author',
                dataIndex: 'value',
                hidden: true,
                width: 200

            }, {
                text: 'Date (Server Time)',
                dataIndex: 'timestamp',
		renderer: this.formatDate,
                width: 200
            }]
        });

	this.new_data = Ext.create('Ext.data.Store', {
                model: 'FeedEvent',
                proxy: {
                    type: 'ajax',
                    url: 'statement_view.php?server_id=' + this.server_id + '&row_id=' + this.urlencode(this.max_row_id),
                    reader: {
                        type: 'xml',
                        record: 'event'
                    }
                },
                listeners: {
                    load: this.newDataLoaded,
                    scope: this
                }
            });

        this.callParent(arguments);
        this.on('selectionchange', this.onSelect, this);
    },
	newDataLoaded: function(store, records){
		if(this.store.getAt(0) != null && this.store.getAt(0).get('row_id') == '0' && this.new_data.count() > 0){
			this.store.removeAt(0);
		}
		while(this.new_data.count() > 0){
			this.store.insert(0,this.new_data.getAt(0));
			this.new_data.removeAt(0);
		}
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
        this.getSelectionModel().select(0);
	if(this.max_row_id == '-1'){
		this.createRefreshTask();
	}

    },

    createRefreshTask: function(){
	this.refresh_task = {
	   run: function() {
		this.max_row_id = this.store.getAt(0).get('row_id');
		this.new_data.getProxy().url = 'statement_view.php?server_id=' + this.server_id + '&row_id=' + this.urlencode(this.max_row_id);
		this.new_data.load()
		//var rec = new FeedEvent({'row_id' : '', 'src_ip' : '', 'src_port' : '', 'timestamp':  '', 'dbname' : '', 'appname' : '','username' : '','username' : '','statement' : '','freqency' : '','security_risk' : '','tables' : '','columns' : ''});
		//this.store.insert(0,rec);

		//var new_queries = 
	   },
	   scope: this,
	   interval: 5000 //1 second
	}
	 
	this.runner = new Ext.util.TaskRunner();
	 
	this.runner.start(this.refresh_task);
     },
    /**
     * Instructs the grid to load a new feed
     * @param {String} url The url to load
     */
    loadFeed: function(url){
        var store = this.store;
        store.getProxy().extraParams.feed = url;
        store.load();
    },

    /**
     * Title renderer
     * @private
     */
    formatTitle: function(value, p, record){
        return Ext.String.format('<div class="topic"><b>{0}</b><span class="author">{1}</span></div>', value, 'Database: ' + record.get('dbname') + " &nbsp;&nbsp;&nbsp; Username:" + record.get('username') + " &nbsp;&nbsp;&nbsp; Frequency:" + record.get('freqency') + " &nbsp;&nbsp;&nbsp; Security Risk:" + record.get('security_risk'));
    },

    /**
     * Date renderer
     * @private
     */
    formatDate: function(date){
        if (!date) {
            return '';
        }

        var now = new Date(), d = Ext.Date.clearTime(now, true), notime = Ext.Date.clearTime(date, true).getTime();
        if (notime === d.getTime()) {
            return 'Today ' + Ext.Date.format(date, 'g:i a');
        }

        d = Ext.Date.add(d, 'd', -6);
        if (d.getTime() <= notime) {
            return Ext.Date.format(date, 'D g:i a');
        }
        return Ext.Date.format(date, 'Y/m/d g:i a');
    },

    urlencode : function(str) {
	return escape(str).replace('+', '%2B').replace('%20', '+').replace('*', '%2A').replace('/', '%2F').replace('@', '%40');
     }
});

