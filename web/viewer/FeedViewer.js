/*

This file is part of Ext JS 4

Copyright (c) 2011 Sencha Inc

Contact:  http://www.sencha.com/contact

GNU General Public License Usage
This file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE included in the packaging of this file.  Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.

If you are unsure which license is appropriate for your use, please contact the sales department at http://www.sencha.com/contact.

*/
/**
 * @class FeedViewer.FeedViewer
 * @extends Ext.container.Viewport
 *
 * The main FeedViewer application
 * 
 * @constructor
 * Create a new Feed Viewer app
 * @param {Object} config The config object
 */

Ext.Loader.setConfig({ enabled: true});
Ext.Loader.setPath('Ext.multisort', 'multisort/');
Ext.Loader.setPath('Ext.ux', 'ux/');

Ext.require([
    'Ext.data.Store',
    'Ext.data.proxy.Ajax',
    'Ext.multisort.Panel',
    'Ext.multisort.SortButton',
    'Ext.ux.BoxReorderer',
    'Ext.ux.DataView.Animated'
]);


Ext.define('FeedViewer.App', {
    extend: 'Ext.container.Viewport',
  
    initComponent: function(){
        
        Ext.define('Feed', {
            extend: 'Ext.data.Model',
            fields: ['id', 'server_name','server_id'],
proxy: {
            		type: 'rest',
            		url: 'tonic/server'
                        }
        });

        
        Ext.apply(this, {
            layout: 'border',
            padding: 5,
            items: [this.createFeedPanel(), this.createFeedInfo(),this.createAlertPanel()]
        });

	this.feedInfo.add(Ext.create('Ext.multisort.Panel', {region: 'center',minWidth: 300,width:400}));



        this.callParent(arguments);
    },
    
    /**
     * Create the list of fields to be shown on the left
     * @private
     * @return {FeedViewer.FeedPanel} feedPanel
     */
    createFeedPanel: function(){
        this.feedPanel = Ext.create('widget.feedpanel', {
            region: 'west',
            collapsible: true,
            width: 245,
            floatable: false,
            split: true,
            minWidth: 175,
            listeners: {
                scope: this,
                feedselect: this.onFeedSelect
            }
        });
        return this.feedPanel;
    },
    
    /**
     * Create the feed info container
     * @private
     * @return {FeedViewer.FeedInfo} feedInfo
     */
    createFeedInfo: function(){
        this.feedInfo = Ext.create('widget.feedinfo', {
            region: 'center',
            minWidth: 300
        });
        return this.feedInfo;
    },

    createAlertPanel: function(){
    Ext.define('Message', {
        extend: 'Ext.data.Model',
        fields: [
            'id', 'message_type', {name:'message_timestamp',type:'date', dateFormat:'timestamp'},'message_text'
        ]
    });

    // create the Data Store
    this.message_store = Ext.create('Ext.data.Store', {
        pageSize: 50,
        model: 'Message',
        remoteSort: true,
       autoLoad: true,
        autoSync: true,
        proxy: {
            // load using script tags for cross domain, if the data in on the same domain as
            // this page, an HttpProxy would be better
            type: 'rest',
            url: 'tonic/messages',
            		reader: {
                		type: 'json',
		                root: 'data'
            		},
 		        writer: {
                		type: 'json'
		        }
        }

    });

    function renderSeverity(value, p, record){
	if(value == 'INFO'){
		return Ext.String.format('<img src="images/rss_add.gif"/>');
	}
	if(value == 'ALERT'){
		return Ext.String.format('<img src="images/rss_delete.gif"/>');
	}
	alert(value);
    }

    function renderTopic(value, p, record) {
        return Ext.String.format(
            '<b><a href="#">{0}</a></b>{1}',
            value,
            record.data.message_timestamp
        );
    }


        this.alertPanel = Ext.create('Ext.grid.Panel', {
            region: 'east',
            minWidth: 300,
	    width: 250,
	    store: this.message_store,

columns:[{
            // id assigned so we can apply custom css (e.g. .x-grid-cell-topic b { color:#333 })
            // TODO: This poses an issue in subclasses of Grid now because Headers are now Components
            // therefore the id will be registered in the ComponentManager and conflict. Need a way to
            // add additional CSS classes to the rendered cells.
            id: 'severity',
            text: "Serverity",
	    renderer: renderSeverity,
	     width: 56,
            dataIndex: 'message_type',
            sortable: false
        },{
	    id: 'topic',
            text: "Event",
            dataIndex: 'message_text',
	    renderer: renderTopic,
	      flex: 1,
            width: 230,
            align: 'left',
            sortable: true
        }],
        // paging bar on the bottom
        bbar: Ext.create('Ext.PagingToolbar', {
            store: this.message_store

           })
        });
        return this.alertPanel;
     },


    
    /**
     * Reacts to a feed being selected
     * @private
     */

     addAppPanel: function(panel){
        this.feedInfo.add(panel).show();
	},

    onFeedSelect: function(feed, title, url){
	if(this.checkAppIsOpen(title)){
        	this.feedInfo.addFeed(title, url);
	}
    },

    checkAppIsOpen: function(title){

	for(x in this.feedInfo.items.items){
		if(this.feedInfo.items.items[x].title == title){
			this.feedInfo.items.items[x].show();
			return false;
		}
	}
	return true;

    },

    launchServerStatementApp: function(title,id){
	if(this.checkAppIsOpen(title)){
        	this.feedInfo.addFeed(title, id);
	}
    },

    launchPolicyApp: function(){
		if(this.checkAppIsOpen('Policies')){
			var policy = new Policy.App();
			policy.setApp(this);
			policy.launchApp();
		}
    },

    launchUserApp: function(){

		if(this.checkAppIsOpen('Users')){
			var user = new User.App();
			user.setApp(this);
			user.launchApp();
		}
    },

    launchCleanDataApp: function(){
		var cleanData = new CleanData.App();
		cleanData.show();
    },
    launchDownloadDataApp: function(){
		var downloadData = new DownloadData.App();
		downloadData.show();
    },
    launchSupportApp: function(){
		if(this.checkAppIsOpen('Support')){
			this.feedInfo.add({title    : 'Support',
			 	html     : '<iframe id="tree" name="tree" src="/index.php/contact-and-support/tech-support" frameborder="0" marginheight="0" marginwidth="0" width="100%" height="100%" scrolling="auto">',
			 	closable : true ,
			 	id: 'support',
		    	}).show();
		}
   },

   launchLogoutApp: function(){
		window.location = '/index.php/logout';
   },
   launchFAQApp: function(){
		if(this.checkAppIsOpen('FAQ')){
			this.feedInfo.add({title    : 'FAQ',
			 	html     : '<iframe id="tree" name="tree" src="/index.php?option=com_fsf&view=faq" frameborder="0" marginheight="0" marginwidth="0" width="100%" height="100%" scrolling="auto">',
			 	closable : true ,
			 	id: 'FAQ',
		    	}).show();
		}
  },

  refresh: function(){
	this.message_store.load();
  }
});

