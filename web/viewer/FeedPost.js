/*

This file is part of Ext JS 4

Copyright (c) 2011 Sencha Inc

Contact:  http://www.sencha.com/contact

GNU General Public License Usage
This file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE included in the packaging of this file.  Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.

If you are unsure which license is appropriate for your use, please contact the sales department at http://www.sencha.com/contact.

*/
/**
 * @class FeedViewer.FeedPost
 * @extends Ext.panel.Panel
 *
 * Shows the detail of a feed post
 *
 * @constructor
 * Create a new Feed Post
 * @param {Object} config The config object
 */
Ext.define('FeedViewer.FeedPost', {

    extend: 'Ext.panel.Panel',
    alias: 'widget.feedpost',
    cls: 'preview',
    autoScroll: true,
    border: true,
    
    initComponent: function(){
        Ext.apply(this, {
            dockedItems: [this.createToolbar()],
            tpl: Ext.create('Ext.XTemplate',
                '<div class="post-data">',
                    '<h3 class="post-title">{timestamp:this.formatDate}</h3>',
                '</div>',


                '<table width="100%">',
			'<tr>',
				'<td width="200">Row ID</td><td>{row_id}</td>',
			'</tr>',
			'<tr>',
				'<td>Statement</td><td>{statement}</td>',
			'</tr>',
			'<tr>',
				'<td>Database</td><td>{dbname}</td>',
			'</tr>',
			'<tr>',
				'<td>Username</td><td>{username}</td>',
			'</tr>',
			'<tr>',
				'<td>Application Name</td><td>{appname}</td>',
			'</tr>',
			'<tr>',
				'<td>IP Address</td><td>{src_ip}:{src_port}</td>',
			'</tr>',
			'<tr>',
				'<td>Frequency</td><td>{freqency}</td>',
			'</tr>',
			'<tr>',
				'<td>Security Risk</td><td>{security_risk}</td>',
			'</tr>',
			'<tr>',
				'<td>Tables</td><td>{tables}</td>',
			'</tr>',
			'<tr>',
				'<td>Columns</td><td>{columns}</td>',
			'</tr>',
			'<tr>',
				'<td>Similar Statements</td><td>{similar_statements}</td>',
			'</tr>',
		'</table>',


//'row_id', 'src_ip', 'src_port', {name:'timestamp',type:'date', dateFormat:'timestamp'}, 'dbname', 'appname','username','username','statement','freqency','security_risk','tables','columns'
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

    /**
     * Set the active post
     * @param {Ext.data.Model} rec The record
     */
    setActive: function(rec) {
        this.active = rec;
        this.update(rec.data);
    },

    /**
     * Create the top toolbar
     * @private
     * @return {Ext.toolbar.Toolbar} toolbar
     */
    createToolbar: function(){
        var items = [],
            config = {};
            items.push({
                scope: this,
                handler: this.openTab,
                text: 'Save Statement As Report',
                iconCls: 'tab-new'
            });
        config.items = items;
        return Ext.create('widget.toolbar', config);
    },

    /**
     * Open the post in a new tab
     * @private
     */
    openTab: function(){
        this.fireEvent('opentab', this, this.active);
    }

});

