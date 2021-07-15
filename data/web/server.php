<?php


require 'lib.php';

if(!$u || !$p || !$wid) die("[ERROR]srvmsg_params\0");


db_open();


$u1 = str_replace('"','""',$u);
$result = db_query('select id,password from wot_accounts where user="'.$u1.'"') or die("[ERROR]srvmsg_query\0");
if(!($row=mysql_fetch_array($result,MYSQL_NUM)) || !$row[0]) die("[ERROR]srvmsg_usr\0");
if(!$p || $row[1]!=$p) die("[ERROR]srvmsg_pwd\0");
$uid = $row[0];

$ip = $_SERVER['REMOTE_ADDR'];
$o = $o+0;

db_query('update wot_worlds set status="'.($c=='stop'? '0' : '1').
	'",players="'.$o.'",ip="'.$ip.'" where id='.$wid.' and uid='.$uid) or die("[ERROR]srvmsg_query\0");

$result = db_query('select * from wot_worlds where status="1" order by name') or die("[ERROR]srvmsg_query\0");
$fp = fopen('hosts.dat','w');
while($row=mysql_fetch_array($result,MYSQL_ASSOC)) {
	fwrite($fp,$row['id'].';'.$row['name'].';'.$row['ip'].';'.$row['players'].";0\n");
}
fclose($fp);
echo "OK\0";

?>