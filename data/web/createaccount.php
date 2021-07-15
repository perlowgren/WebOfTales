<?php


require 'lib.php';

if(!$u || !$p) die("[ERROR]srvmsg_params\0");


db_open();

$u1 = str_replace('"','""',$u);
$result = db_query('select id from wot_accounts where user="'.$u1.'"');
if(($row=mysql_fetch_array($result,MYSQL_NUM)) && $row[0]) die("[ERROR]srvmsg_unmchk\0");

if($c=='create') {
	$p1 = str_replace('"','""',$p);
	$e1 = str_replace('"','""',$e);
	$nm1 = str_replace('"','""',$nm);
	$ln1 = str_replace('"','""',$ln);
	$ip = $_SERVER['REMOTE_ADDR'];
	$result = db_query('insert into wot_accounts values(null,"'.$u1.'","'.$p1.'","'.$e1.'","'.$ip.'",'.
		'"2","'.$nm1.'","'.$ln1.'","","0","0","0","0","0",now())') or die("[ERROR]srvmsg_query\0");
	die('uid='.mysql_insert_id()."\0");
}
echo "OK\0";

?>