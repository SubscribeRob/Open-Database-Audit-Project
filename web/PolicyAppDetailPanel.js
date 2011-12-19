/*
PolicyApp.DetailPanel allows users to edit, display, and scrool through the history of 
events that have occured. This panel is below the grid panel

*/

Ext.define('PolicyApp.DetailPanel', {
	extend: 'Ext.panel.Panel',
	alias:  'policy.detailpanel',
	cls:	'policy',
	autoScroll: true,
	border: true,
	notifications: null,


    initComponent: function(){


        Ext.apply(this, {
            dockedItems: [this.createToolbar()],
            tpl: Ext.create('Ext.XTemplate',
                '<br /><table cellspacing="10"><tr><td width="20">&nbsp;</td><td><table cellspacing="10">',
		    '<tr><td colspan="2"><center><b>Policy Conditions For Alert</b></center></td></tr>',
                    '<tr><td width="100">Policy Name:</td><td>{job_name}</td></tr>',
		    '<tr><td>Pre Processor:</td><td>{pre_processor}</td></tr>',
		    '<tr><td>Post Processor:</td><td>{post_processor}</td></tr>',
		    '<tr><td>Tables:</td><td>{tables}</td></tr>',
   		    '<tr><td>Users:</td><td>{users}</td></tr>',
		    '<tr><td>IP Addresses:</td><td>{ips}</td></tr>',
		    '<tr><td>Ports:</td><td>{ports}</td></tr>',
		    '<tr><td>Columns:</td><td>{columns}</td></tr>',
		    '<tr><td>Create SQL:</td><td>{create_sql}</td></tr>',
		    '<tr><td>Drop SQL:</td><td>{drop_sql}</td></tr>',
		    '<tr><td>Alter SQL:</td><td>{alter_sql}</td></tr>',
		    '<tr><td>Grant SQL:</td><td>{grant_sql}</td></tr>',
		    '<tr><td>Revoke SQL:</td><td>{revoke_sql}</td></tr>',
		    '<tr><td>Insert SQL:</td><td>{insert_sql}</td></tr>',
		    '<tr><td>Update SQL:</td><td>{update_sql}</td></tr>',
		    '<tr><td>Delete SQL:</td><td>{delete_sql}</td></tr>',
		    '<tr><td>Backup SQL:</td><td>{backup_sql}</td></tr>',

		    '</table></td><td width="80">&nbsp;</td><td  valign="top"><table>',
		    
		    '<tr><td valign="top" colspan="2"><center><b>Policy Run Statistics</b></center></td></tr>',
		    '<tr><td>Last Run Start Time:</td><td>{last_run_timestamp}</td></tr>',
		    '<tr><td>Last Run End Time:</td><td>{last_run_timestamp_end}</td></tr>',
		    '<tr><td>Schedule:</td><td>{cron}</td></tr>',
		    '<tr><td>Processing Server:</td><td>{last_run_name}</td></tr>',
		    '<tr><td>Max Key Processed:</td><td>{last_run_key_value_max}</td></tr>',

	  	    '</table></td><td width="80">&nbsp;</td>',


		    '</table></td>',

		    '</tr></table>',
                {
                    getBody: function(value, all){
                        return Ext.util.Format.stripScripts(value);
                    },

                    defaultValue: function(v){
                        return v ? v : 'Unknown';
                    },

                    formatDate: function(value){
                        if (!value) {
                            return '';
                        }
                        return Ext.Date.format(value, 'M j, Y, g:i a');
                    }
                }
             )
        });
        this.callParent(arguments);
    },

    setActive: function(rec) {

        this.active = rec;
        this.update(rec.data);
    },

    createToolbar: function(){
        var items = [],
            config = {};
        if (!this.inTab) {
            items.push({
                scope: this,
                handler: this.openCreatePolicyAlert,
                text: 'Create Full Policy Report',
                iconCls: 'tab-new'
            }, '-');
        }
        else {
            config.cls = 'x-docked-noborder-top';
        }
            items.push({
                scope: this,
                handler: this.openCreatePolicyAlert,
                text: 'User Notification List',
            }, '-');
        config.items = items;
        return Ext.create('widget.toolbar', config);
    },

    goToPost: function(){
        window.open(this.active.get('link'));
    },

    openCreatePolicyAlert: function(){
	alert('Need to implement');
        //this.fireEvent('opentab', this, this.active);
    },
    onLoad: function(){
	//alert('LOADED');
        //this.getSelectionModel().select(0);
    },


//End PolicyApp.DetailPanel
});
