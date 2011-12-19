<?php
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
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>DB Audit Cloud</title>
    <link rel="stylesheet" type="text/css" href="resources/css/ext-all.css">
    <link rel="stylesheet" type="text/css" href="Feed-Viewer.css">

        <link rel="stylesheet" type="text/css" href="multisort/multisort.css" />


<style type="text/css">
.x-menu-item img.preview-right, .preview-right {
    background-image: url(images/preview-right.gif);
}
.x-menu-item img.preview-bottom, .preview-bottom {
    background-image: url(images/preview-bottom.gif);
}
.x-menu-item img.preview-hide, .preview-hide {
    background-image: url(images/preview-hide.gif);
}

#reading-menu .x-menu-item-checked {
    border: 1px dotted #a3bae9 !important;
    background: #DFE8F6;
    padding: 0;
    margin: 0;
}

        .x-grid-cell-topic b {
            display: block;
        }
        .x-grid-cell-topic .x-grid-cell-inner {
            white-space: normal;
        }
        .x-grid-cell-topic a {
            color: #385F95;
            text-decoration: none;
        }
        .x-grid-cell-topic a:hover {
            text-decoration:underline;
        }
		.x-grid-cell-topic .x-grid-cell-innerf {
			padding: 5px;
		}
		.x-grid-rowbody {
	        padding: 0 5px 5px 5px; 
		}
</style>
    <script type="text/javascript" src="bootstrap.js"></script>
    <script type="text/javascript" src="viewer/FeedPost.js"></script>
    <script type="text/javascript" src="viewer/FeedDetail.js"></script>
    <script type="text/javascript" src="viewer/FeedGrid.js"></script>
    <script type="text/javascript" src="viewer/FeedInfo.js"></script>
    <script type="text/javascript" src="viewer/FeedPanel.js"></script>
    <script type="text/javascript" src="viewer/FeedViewer.js"></script>
    <script type="text/javascript" src="viewer/FeedWindow.js"></script>

    <script type="text/javascript" src="UserApp.js"></script>
    <script type="text/javascript" src="DownloadDataApp.js"></script>
    <script type="text/javascript" src="CleanDataApp.js"></script>
    <script type="text/javascript" src="PolicyAppDetailPanel.js"></script>
    <script type="text/javascript" src="PolicyAppGridPanel.js"></script>
    <script type="text/javascript" src="PolicyApp.js"></script>
    <script type="text/javascript" src="PolicyCreatePolicy.js"></script>

    <script type="text/javascript">
        Ext.Loader.setConfig({enabled: true});
        Ext.Loader.setPath('Ext.ux', 'ux/');
        Ext.require([
            'Ext.grid.*',
            'Ext.data.*',
            'Ext.util.*',
            'Ext.Action',
            'Ext.tab.*',
            'Ext.button.*',
            'Ext.form.*',
            'Ext.layout.container.Card',
            'Ext.layout.container.Border',
            'Ext.ux.PreviewPlugin',
	    'Policy.App',
    	    'Ext.tip.QuickTipManager',
            'Ext.ux.LiveSearchGridPanel'
        ]);

	var app = null;
        Ext.onReady(function(){
	
		app = new FeedViewer.App();

		
        });
    </script>
</head>
<body>
</body>
</html>
