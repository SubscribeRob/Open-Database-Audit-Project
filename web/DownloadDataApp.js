Ext.define('DownloadData.App', {
	extend: 'Ext.window.Window',

	alias: 'downloaddata.window',
	plain: true,

	initComponent: function(){

	Ext.apply(Ext.form.field.VTypes, {
		daterange: function(val, field) {
		    var date = field.parseDate(val);

		    if (!date) {
		        return false;
		    }
		    if (field.startDateField && (!this.dateRangeMax || (date.getTime() != this.dateRangeMax.getTime()))) {
		        var start = field.up('form').down('#' + field.startDateField);
		        start.setMaxValue(date);
		        start.validate();
		        this.dateRangeMax = date;
		    }
		    else if (field.endDateField && (!this.dateRangeMin || (date.getTime() != this.dateRangeMin.getTime()))) {
		        var end = field.up('form').down('#' + field.endDateField);
		        end.setMinValue(date);
		        end.validate();
		        this.dateRangeMin = date;
		    }
		    /*
		     * Always return true since we're only using this vtype to set the
		     * min/max allowed values (these are tested for after the vtype test)
		     */
		    return true;
		},

		daterangeText: 'Start date must be less than end date'

	});

		var sm = new Ext.selection.CheckboxModel( {
		    listeners:{
			selectionchange: function(selectionModel, selected, options){
			    alert('test');
			}
		    }
		});

		this.form = Ext.create('Ext.form.Panel', {
			width: 400,
			frame:true,
			items: [{
				    xtype: 'combo',
				    fieldLabel: 'Server',
				    name: 'server',
				    boxLabel: 'Server',
           			    valueField: 'id',
            			    displayField: 'server_name',
				    id: 'server',
				    allowBlank: false,
				    store: Ext.create('Ext.data.Store', {
							model: 'Feed',
							remoteSort: true,
							autoLoad: true,
							autoSync: true,
							proxy: {
							    // load using script tags for cross domain, if the data in on the same domain as
							    // this page, an HttpProxy would be better
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

			},{
				xtype: 'checkbox',
				fieldLabel: 'Download All Data',
				checked: true,
				name: 'download_all_data',
				id: 'all',
				inputValue: 'all'
			},{
		                fieldLabel: 'Start Date',
				xtype: 'datefield',
		                name: 'startdt',
		                id: 'startdt',
				disabled: true,
		                vtype: 'daterange',
                		endDateField: 'enddt' // id of the end date field
            		},{
		                fieldLabel: 'End Date',
				xtype: 'datefield',
                		name: 'enddt',
				disabled: true,
                		id: 'enddt',
                		vtype: 'daterange',
		                startDateField: 'startdt' // id of the start date field
            		},{
				fieldLabel: 'Format',
				xtype: 'combo',
				editable: false,
				name: 'format',
				id: 'format',
                                displayField:   'name',
                                valueField:     'value',
                                queryMode: 'local',
                                store:          Ext.create('Ext.data.Store', {
                                    fields : ['name', 'value'],
                                    data   : [
                                        {name : '.gz',   value: '.gz'}
                                    ]
                                })

			}]


		});

		Ext.getCmp('all').on('change', function(){    
		    if (this.checked) {
			Ext.getCmp('startdt').disable(true);
			Ext.getCmp('enddt').disable(true);
		    }else{
			Ext.getCmp('startdt').enable(false);
			Ext.getCmp('enddt').enable(false);
		   }
		});

		Ext.apply(this, {
		    title: 'Download Server Data',
		    layout: 'fit',
		    items: this.form,
		    buttons: [{
		        xtype: 'button',
		        text: 'Download Server Data',
		        scope: this,
		        handler: this.onDownloadClick
		    }, {
		        xtype: 'button',
		        text: 'Cancel',
		        scope: this,
		        handler: this.destroy
		    }]
		});

		Ext.getCmp('format').setValue('.gz');
		this.callParent(arguments);
	},

	onDownloadClick: function(){
		if(this.form.getForm().isValid()){
			this.form.setLoading({
			    msg: 'Validating download data request...'
			});

			var server		= this.form.getComponent('server').getValue();
			var o_startdt = null;
			var o_enddt = null;
			var all_data = null;
			var format = this.form.getComponent('format').getValue();

			if(Ext.getCmp('all').checked == false){
				o_startdt = this.form.getComponent('startdt').getValue();
				o_enddt =  this.form.getComponent('enddt').getValue();
			}else{
				all_data = this.form.getComponent('all').getValue();
			}

		
			Ext.Ajax.request({
			    url: 'tonic/jobs',
			    params: {
				server_id:server,
				preprocessor: 'com.dbauditcloud.job.DownloadServerData',
				start_date: o_startdt,
				end_date: o_enddt,
				all_data: all_data,
				format: format
			    },
			    success: this.validRequest,
			    failure: this.invalidRequest,
			    scope: this
			});
		}
	},

	validRequest: function(){
		this.form.setLoading(false);
		Ext.Msg.alert('Download Data Request', 'The job submission to download your data has been submitted successfully. Over the next 24 hours ( if under 10 TB ) you should recieve an e-mail with a url to download your data');
		this.destroy();
		app.refresh();
	},

	invalidRequest: function(){
		this.form.setLoading(false);
		Ext.Msg.alert('Download Data Request', 'Failed! The job request to download data was denied. Please try again later');
		this.destroy();
	},

	onChecked: function(){ alert('test'); }
});
