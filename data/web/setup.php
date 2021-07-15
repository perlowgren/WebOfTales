<?php


require 'lib.php';


db_open();

$q = "CREATE TABLE `wot_accounts` (
  `id` int(11) NOT NULL auto_increment,
  `user` varchar(32) default '',
  `password` varchar(32) default '',
  `ip` varchar(16) default '',
  `access` int(11) default '2',
  `name` varchar(32) default '',
  `lang` varchar(2) default 'en',
  `country` varchar(2) default '',
  `status` int(11) default '0',
  `visits` int(11) default '0',
  `online` int(11) default '0',
  `login` datetime NOT NULL default '0000-00-00 00:00:00',
  `logout` datetime NOT NULL default '0000-00-00 00:00:00',
  `created` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  KEY `user` (`user`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=156 DEFAULT CHARSET=utf8";
echo "<p>Create table 'wot_accounts'<br><pre>$q</pre></p>";
db_query($q);

$query = "CREATE TABLE `wot_worlds` (
  `id` int(11) NOT NULL auto_increment,
  `uid` int(11) default '0',
  `name` varchar(32) default '',
  `status` int(11) default '0',
  `players` int(11) default '0',
  `ip` varchar(16) default '',
  `online` int(11) default '0',
  `login` datetime NOT NULL default '0000-00-00 00:00:00',
  `logout` datetime NOT NULL default '0000-00-00 00:00:00',
  `created` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  KEY `uid` (`uid`),
  KEY `name` (`name`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=156 DEFAULT CHARSET=utf8";
echo "<p>Create table 'wot_worlds'<br><pre>$query</pre></p>";
db_query($query);

$query = "CREATE TABLE `wot_chars` (
  `id` int(11) NOT NULL auto_increment,
  `uid` int(11) default '0',
  `name` varchar(32) default '',
  `status` int(11) default '0',
  `ip` varchar(16) default '',
  `xp` int(11) default '0',
  `lvl` int(11) default '0',
  `st` int(11) default '0',
  `div` int(11) default '0',
  `s` int(11) default '0',
  `f` int(11) default '0',
  `a` int(11) default '0',
  `w` int(11) default '0',
  `e` int(11) default '0',
  `align` tinyint(4) default '0',
  `age` int(11) default '0',
  `gen` enum('none','female','male','androgyne') default 'none',
  `race` tinyint(4) default '0',
  `online` int(11) default '0',
  `login` datetime NOT NULL default '0000-00-00 00:00:00',
  `logout` datetime NOT NULL default '0000-00-00 00:00:00',
  `created` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  KEY `uid` (`uid`),
  KEY `name` (`name`),
  KEY `status` (`status`)
) ENGINE=MyISAM AUTO_INCREMENT=156 DEFAULT CHARSET=utf8";
echo "<p>Create table 'wot_chars'<br><pre>$query</pre></p>";
db_query($query);

echo '<script>alert("Web of Tales webserver installed and ready.");</script>';
