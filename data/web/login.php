<?php


require 'lib.php';

if(!$u || !$p) die("[ERROR]srvmsg_params\0");


db_open();

$u1 = str_replace('"','""',$u);
$result = db_query('select id,password from wot_accounts where user="'.$u1.'"') or die("[ERROR]srvmsg_query\0");
if(!($row=mysql_fetch_array($result,MYSQL_NUM)) || !$row[0]) die("[ERROR]srvmsg_usr\0");
if(!$p || $row[1]!=$p) die("[ERROR]srvmsg_pwd\0");
$uid = $row[0];

$ip = $_SERVER['REMOTE_ADDR'];
$port = 4444;
echo 'localhost='.$ip."\nlocal_addr=".$ip.':'.$port.' '.$uid.'|'.$u."\nuser=".$u."\npassword=".$p."\nid=".$uid."\n\0";

?>