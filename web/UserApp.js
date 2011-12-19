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

Ext.define('User.Model',{
	extend: 'Ext.data.Model',
	fields: ['id','user_name','password','e_mail','account_type','account_id']
});

Ext.define('User.App', {

    extend: 'Ext.grid.Panel',
    alias: 'users.panel',

    requires: [
        'Ext.grid.plugin.CellEditing',
        'Ext.form.field.Text',
        'Ext.toolbar.TextItem'
    ],

    layout: 'fit',
    title: 'Users',
    closable: true,
    app: null,

    setApp: function (app_ptr){
		app = app_ptr;
    },
    launchApp: function(){
		app.addAppPanel(this);
     },

    initComponent: function(){
 	this.editing = Ext.create('Ext.grid.plugin.CellEditing');

	Ext.apply(this,{

		plugins: [this.editing]
	
	});
	this.store = Ext.create('Ext.data.Store', {
				model: 'User.Model',
				remoteSort: true,
				autoLoad: true,
				autoSync: true,
				proxy: {
				    // load using script tags for cross domain, if the data in on the same domain as
				    // this page, an HttpProxy would be better
				    type: 'rest',
				    url: 'tonic/users',
				    		reader: {
							type: 'json',
							root: 'data'
				    		},
			 		        writer: {
							type: 'json'
						}
				}
            });//'user_name','password','e_mail','account_type','account_id'
 
        this.dockedItems = [Ext.create('widget.toolbar', {
                xtype: 'toolbar',
                items: [{
                    iconCls: 'icon-add',
                    text: 'Add',
                    scope: this,
                    handler: this.onAddClick
                }, {
                    iconCls: 'icon-delete',
                    text: 'Delete',
                    disabled: true,
                    itemId: 'delete',
                    scope: this,
                    handler: this.onDeleteClick
            }, {
                    iconCls: 'icon-edit',
                    text: 'Edit',
                    disabled: true,
                    itemId: 'edit',
                    scope: this,
                    handler: this.onEditClick
            }]
        })];

        this.callParent();
	this.getSelectionModel().on('selectionchange', this.onSelectChange, this);
    },
	columns: [{
                text: 'User ID',
                dataIndex: 'id',
                width: 75

            },{
                text: 'User Name',
                dataIndex: 'user_name',
                width: 125,
                field: {
                    type: 'textfield'
                }

            }, {
                text: 'E-Mail',
                dataIndex: 'e_mail',
                width: 150,
                field: {
                    type: 'textfield'
                }
            }],




    onDeleteClick: function(){
        var selection = this.getView().getSelectionModel().getSelection()[0];
        if (selection) {
            this.store.remove(selection);
        }
	app.refresh();
    },

    onAddClick: function(){
		    this.form = Ext.create('Ext.form.Panel', {
			frame:true,
			bodyStyle:'padding:5px 5px 0',
			width: 350,
			fieldDefaults: {
			    msgTarget: 'side',
			    labelWidth: 125 
			},
			defaultType: 'textfield',
			defaults: {
			    anchor: '100%'
			},

			items: [{
			    fieldLabel: 'Username',
			    name: 'user_name',
			    id: 'user_name',
			    allowBlank:false
			},{
			    fieldLabel: 'Password',
			    name: 'password',
			    id: 'password',
			    inputType: 'password',
			    allowBlank:false
			},{
			    fieldLabel: 'E-Mail',
			    name: 'e_mail',
			    id: 'e_mail',
			    allowBlank:false
			}
			]

		 });


		this.window_modify = Ext.create('Ext.window.Window',{
			plain: true,
			title: 'Add User',
			layout: 'fit',
			items: this.form,
			buttons: [{
				type: 'button',
				text: 'Add User',
				scope: this,
				handler: this.addUserClick
			}, {
				xtype: 'button',
				text: 'Cancel',
				scope: this,
				handler: this.destoryWindowClick
			}],


		}).show();

    },

    onEditClick: function(){
		    var selection = this.getView().getSelectionModel().getSelection()[0];
		    var username = '';
		    var id = -1;
		    var e_mail = '';
			if (selection) {
			    username = selection.get('user_name');
			    e_mail = selection.get('e_mail');
			    id =  selection.get('id');
			}
		    this.form = Ext.create('Ext.form.Panel', {
			frame:true,
			bodyStyle:'padding:5px 5px 0',
			width: 350,
			fieldDefaults: {
			    msgTarget: 'side',
			    labelWidth: 125 
			},
			defaultType: 'textfield',
			defaults: {
			    anchor: '100%'
			},

			items: [{
			    fieldLabel: 'Username',
			    name: 'user_name',
			    value: username,
			    id: 'user_name',
			    allowBlank:false
			},{
			    fieldLabel: 'Password',
			    name: 'password',
			    id: 'password',
			    value: '12345',
			    inputType: 'password',
			    allowBlank:false
			},{
			    fieldLabel: 'E-Mail',
			    name: 'e_mail',
			    value: e_mail,
			    id: 'e_mail',
			    allowBlank:false
			},{
			    name: 'id',
			    value: id,
			    id: 'id',
			    inputType: 'hidden'
			}
			]

		 });


		this.window_modify = Ext.create('Ext.window.Window',{
			plain: true,
			title: 'Edit User',
			layout: 'fit',
			items: this.form,
			buttons: [{
				type: 'button',
				text: 'Edit User',
				scope: this,
				handler: this.editUserClick
			}, {
				xtype: 'button',
				text: 'Cancel',
				scope: this,
				handler: this.destoryWindowClick
			}],


		}).show();

    },
    destoryWindowClick: function(){
	this.window_modify.destroy();
    },

    addUserClick: function(){
        this.form.setLoading({
            msg: 'Validating user...'
        });
        Ext.Ajax.request({
            url: 'tonic/users',
            params: {
                user_name: this.form.getComponent('user_name').getValue(),
		password: this.form.getComponent('password').getValue(),
		e_mail: this.form.getComponent('e_mail').getValue()
            },
            success: this.validateFeed,
            failure: this.markInvalid,
            scope: this
        });
    },

    editUserClick: function(){
        this.form.setLoading({
            msg: 'Validating user...'
        });
        Ext.Ajax.request({
	    method: 'PUT',
            url: 'tonic/users',
            params: {
		id: this.form.getComponent('id').getValue(),
                user_name: this.form.getComponent('user_name').getValue(),
		password: this.form.getComponent('password').getValue(),
		e_mail: this.form.getComponent('e_mail').getValue()
            },
            success: this.validateFeed,
            failure: this.markInvalid,
            scope: this
        });
    },
    validateFeed: function(response){
        this.form.setLoading(false);
	this.destoryWindowClick();
	this.store.load();
	app.refresh();
        return;
    },
    

    markInvalid: function(response, opts) {
	alert(response.responseText);
        this.form.setLoading(false);
    },

    onSelectChange: function(selModel, selections){
        this.down('#delete').setDisabled(selections.length === 0);
	this.down('#edit').setDisabled(selections.length === 0);
    }
});
