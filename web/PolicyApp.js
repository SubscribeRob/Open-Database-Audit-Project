Ext.define('Policy.Model',{
	extend: 'Ext.data.Model',
	fields: ['id','server_id' , 'pre_processor' , 'post_processor' , 'tables', 'users', 'ips' , 'ports', 'columns' , 'create_sql' , 'drop_sql', 'alter_sql', 'grant_sql' , 'revoke_sql' ,
           	 'insert_sql' , 'update_sql' ,  'delete_sql' , 'select_sql' , 'backup_sql' , 'restore_sql' , 'time_start' , 'time_end' ,  'last_run_timestamp' , 'last_run_name' ,
  		 'last_run_key_value_max' , 'create_timestamp' , 'alter_timestamp' , 'cron', 'job_name', 'violations_not_acknowledged', 'violations_acknowledged','violations_total', 'total_statements_analyzed',
		  'total_cpu_user','total_cpu_system', 'last_run_timestamp_end']
//End Policy.Model
});
Ext.define('Notifications',{
	extend: 'Ext.data.Model',
	fields: ['job_id','type','title','body','user_id','acknowledged'],
	belongsTo: 'Policy.Model',
	proxy: {
	    type: 'rest',
	    url: 'tonic/notifications/1',
	    		reader: {
				type: 'json',
				root: 'data'
	    		},
 		        writer: {
				type: 'json'
			}
	    }
});

Ext.define('Policy.App', {
    extend: 'Ext.panel.Panel',
    alias: 'policy.panel',

    layout: 'fit',
    title: 'Policies',
    closable: true,
    //Point To FeedViewerApp
    app: null,

    setApp: function (app_ptr){
		app = app_ptr;
    },
    launchApp: function(){
		app.addAppPanel(this);
     },


    initComponent: function(){

	    this.display = Ext.create('policy.detailpanel', {});
 	    Ext.apply(this, {
		layout: 'border',
            	items: [this.createGrid(),this.createSouth()]
	    });

		
                this.south.show();
                this.south.add(this.display);
	this.callParent(arguments);

    },

    createGrid: function(){

        this.grid = Ext.create('policy.gridpanel', {
            region: 'center',
            dockedItems: [this.createTopToolbar()],
            flex: 2,
            minHeight: 200,
            minWidth: 150,
            listeners: {
                scope: this,
                select: this.onSelect
            }
        });
        return this.grid;
     //END createGrid
     },
    /**
     * Creates top controller toolbar.
     * @private
     * @return {Ext.toolbar.Toolbar} toolbar
     */
    createTopToolbar: function(){
        this.toolbar = Ext.create('widget.toolbar', {
            cls: 'x-docked-noborder-top',
            items: [{
                iconCls: 'icon-add',
                text: 'Create Policy',
                scope: this,
                handler: this.onCreatePolicyClick
            },  {
                iconCls: 'icon-delete',
                text: 'Delete Policy',
                scope: this,
                handler: this.onDeletePolicyClick
            }]
        });
        return this.toolbar;
    },

    /**
     * Fires when a grid row is selected
     * @private
     * @param {FeedViewer.FeedGrid} grid
     * @param {Ext.data.Model} rec
     */
    onSelect: function(grid, rec) {
        this.display.setActive(rec);
    },

    onValidPolicy: function(){
	this.grid.loadPolicies();
    },

    createSouth: function(){
		this.south =  Ext.create('Ext.container.Container', {
            		layout: 'fit',
            		region: 'south',
            		split: true,
            		flex: 2,
            		minHeight: 150,
            		items: this.display
        	});
            return this.south;
    },

   onCreatePolicyClick: function(){
        var win = Ext.create('policy.create_policy_1', {
		listeners: {
                	scope: this,
               		validpolicy: this.onValidPolicy
            	}
        });
        win.show();
    },
	
    onDeletePolicyClick: function(){
        var selection = this.grid.getSelectionModel().getSelection()[0];
        if (selection) {
            this.grid.store.remove(selection);
        }
	app.refresh();
    },
    onPolicyValid: function(){

    }
//END Policy.App
});

