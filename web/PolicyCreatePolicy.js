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

Ext.require([
    'Ext.form.Panel',
    'Ext.ux.form.MultiSelect',
    'Ext.ux.form.ItemSelector'
]);

Ext.define('Policy.CreatePolicy', {

	extend: 'Ext.window.Window',
	alias:  'policy.create_policy_1',
	plain: true,
    	schedule:[['0 0 * * * ?','Daily']],
	initComponent: function(){
		this.addEvents(
		    'validpolicy'
		);

		this.form = Ext.create('Ext.form.Panel', {
			frame:true,
			bodyStyle:'padding:5px 5px 0',
			width: 500,
			fieldDefaults: {
			    msgTarget: 'side',
			    labelWidth: 125 
			},
			defaultType: 'textfield',
			defaults: {
			    anchor: '100%',
                	    hideEmptyLabel: false
			},

			items: [{	fieldLabel: 'Policy Name',
			   		name: 'policy_name',
			    		id: 'policy_name',
			    		allowBlank:false
				},{
				        fieldLabel: 'Server',
				        name: 'server',
				        xtype: 'combo',
					editable: false,
					id: 'server',
					valueField: 'id',
					displayField: 'server_name',
				        store: Ext.create('Ext.data.Store', {
							model: 'Feed',
					    		
							autoLoad: true,
							autoSync: true,
							proxy: {
						    		type: 'rest',
						    		url: 'tonic/server',
						    		reader: {
									type: 'json',
									root: 'data'
						    		},
					 		        writer: {
									type: 'json'
								}
								}
						    })
				},{	fieldLabel: 'Tables',
			   		name: 'table_names',
			    		id: 'table_names'
				},{	fieldLabel: 'Users',
			   		name: 'user_names',
			    		id: 'user_names'
				},{	fieldLabel: 'IP Addresses',
			   		name: 'ip_addresses',
			    		id: 'ip_addresses'
				},{	fieldLabel: 'Ports',
			   		name: 'ports',
			    		id: 'ports'
				},{	fieldLabel: 'Columns',
			   		name: 'columns',
			    		id: 'columns'
				}, {
					fieldLabel: 'Statement Type',
					xtype: 'checkboxgroup',
					columns: 5,
					items : [{boxLabel: 'Select', name: 'select', itemId: 'select'},
                				 {boxLabel: 'Insert', name: 'insert', id: 'insert'},
						{boxLabel: 'Update', name: 'update', id: 'update'},
						{boxLabel: 'Delete', name: 'delete', id: 'delete'},
						{boxLabel: 'Alter', 	name: 'alter', id: 'alter'}]
				    },  {
					xtype: 'checkboxgroup',
					columns: 5,
					items : [ {boxLabel: 'Create', 	name: 'create', id: 'create'},
                				 {boxLabel: 'Drop', 	name: 'drop', id: 'drop'},
						 {boxLabel: 'Grant', 	name: 'grant', id: 'grant'},
						 {boxLabel: 'Revoke', 	name: 'revoke', id: 'revoke'},
						{boxLabel: 'Backup', 	name: 'backup', id: 'backup'}]
				    }, {
					xtype: 'checkboxgroup',
					fieldLabel: 'Require Acknowledgement',
					columns: 5,
					items : [{boxLabel: 'Yes', 	name: 'acknowledgement', id: 'acknowledgement'} ]
				    }, {
				        fieldLabel: 'Schedule',
				        name: 'schedule',
				        xtype: 'combo',
					editable: false,
					id: 'schedule',
				        store: this.schedule
				    },{
					    xtype: 'multiselect',
					    fieldLabel: 'Alert Users',
					    name: 'users_alert',
					    boxLabel: 'Alert Users',
					    height: 100,
                   			    valueField: 'id',
                    			    displayField: 'user_name',
					    id: 'users_alert',
					    allowBlank: false,
					    store: Ext.create('Ext.data.Store', {
								model: 'User.Model',
								remoteSort: true,
								autoLoad: true,
								autoSync: true,
								proxy: {
								    // load using script tags for cross domain, if the data in on the same domain as
								    // this page, an HttpProxy would be better
								    type: 'rest',
								    url: 'tonic/users/1',
								    		reader: {
											type: 'json',
											root: 'data'
								    		},
							 		        writer: {
											type: 'json'
										}
								}
					    }),


					}
			]
		});

		Ext.getCmp('schedule').setValue('0 0 * * * ?');

		Ext.apply(this, {
		    title: 'Create Policy',
		    iconCls: 'feed',
		    layout: 'fit',
		    items: this.form,
		    buttons: [{
		        xtype: 'button',
		        text: 'Next',
		        scope: this,
		        handler: this.onAddClick
		    }, {
		        xtype: 'button',
		        text: 'Cancel',
		        scope: this,
		        handler: this.destroy
		    }]
		});

		this.callParent(arguments);

	},

	onAddClick: function(){

		var policy_name 	= this.form.getComponent('policy_name').getValue();
		var server		= this.form.getComponent('server').getValue();
		var table_names 	= this.form.getComponent('table_names').getValue();
		var user_names		= this.form.getComponent('user_names').getValue();
		var ip_addresses 	= this.form.getComponent('ip_addresses').getValue();
		var port 		= this.form.getComponent('ports').getValue();

		var columns 		= this.form.getComponent('columns').getValue();

		var select 		= Ext.form.CheckboxManager.getByName('select').items[0].getValue();
		var insert 		= Ext.form.CheckboxManager.getByName('insert').items[0].getValue();
		var update 		= Ext.form.CheckboxManager.getByName('update').items[0].getValue();

		var v_delete 		= Ext.form.CheckboxManager.getByName('delete').items[0].getValue();
		var create 		= Ext.form.CheckboxManager.getByName('create').items[0].getValue();
		var drop 		= Ext.form.CheckboxManager.getByName('drop').items[0].getValue();
		var alter 		= Ext.form.CheckboxManager.getByName('alter').items[0].getValue();
		var grant 		= Ext.form.CheckboxManager.getByName('grant').items[0].getValue();
		var revoke 		= Ext.form.CheckboxManager.getByName('revoke').items[0].getValue();
		var backup 		= Ext.form.CheckboxManager.getByName('backup').items[0].getValue();
		var schedule 		= this.form.getComponent('schedule').getValue();
		var users_alert 	= this.form.getComponent('users_alert').getSubmitValue();//Ext.getCmp('users_alert').getRawValue()

		var acknowledgement 	= Ext.form.CheckboxManager.getByName('acknowledgement').items[0].getValue();

		this.form.setLoading({
		    msg: 'Validating server...'
		});

		Ext.Ajax.request({
		    url: 'tonic/jobs',
		    params: {
		        policy_name: policy_name,
			server_id:server,
			tables: table_names,
			users: user_names,
			ips: ip_addresses,
		        ports: port,

		        columns: columns,
			select_sql: select,
			insert_sql: insert,
			update_sql: update,
		        delete_sql: v_delete,

			create_sql: create,
			drop_sql: drop,
			alter_sql: alter,
		        grant_sql: grant,

			revoke_sql: revoke,
			backup_sql: backup,

			cron: schedule,
			users_alert: users_alert,
			acknowledgement: acknowledgement
		    },
		    success: this.validPolicy,
		    failure: this.markInvalid,
		    scope: this
		});

	},

	validPolicy: function(){
		this.form.setLoading(false);
		this.fireEvent('validpolicy', this, 'test2', 'test2');
		this.destroy();
		app.refresh();
	}


});
