<?php


require 'lib.php';

if(!$u || !$p || !$w) die("[ERROR]srvmsg_params\0");


db_open();

$u1 = str_replace('"','""',$u);
$result = db_query('select id,password from wot_accounts where user="'.$u1.'"') or die("[ERROR]srvmsg_query\0");
if(!($row=mysql_fetch_array($result,MYSQL_NUM)) || !$row[0]) die("[ERROR]srvmsg_usr\0");
if(!$p || $row[1]!=$p) die("[ERROR]srvmsg_pwd\0");
$uid = $row[0];

$w1 = str_replace('"','""',$w);
$result = db_query('select id,uid from wot_worlds where name="'.$w1.'"') or die("[ERROR]srvmsg_query\0");
if(($row=mysql_fetch_array($result,MYSQL_NUM)) && $row[0] && $row[1]!=$uid) die("[ERROR]srvmsg_wnmchk\0");
$wid = $row[0];

if($wid && $uid==$row[1]) die('wid='.$wid."\0");

if($c=='create') {
	$result = db_query('insert into wot_worlds values(null,"'.$uid.'","'.$w1.'","0","0","","0","0","0",now())')
			or die("[ERROR]srvmsg_query\0");
	die('wid='.mysql_insert_id()."\0");
}
echo "OK\0";

?>