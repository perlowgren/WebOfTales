<?php


require 'lib.php';

if(!$u || !$p) die("[ERROR]srvmsg_params\0");


db_open();

$u1 = str_replace('"','""',$u);
$result = db_query('select id,password from wot_accounts where user="'.$u1.'"');
if(!($row=mysql_fetch_array($result,MYSQL_NUM)) || !$row[0]) die("[ERROR]srvmsg_usr\0");
if(!$p || $row[1]!=$p) die("[ERROR]srvmsg_pwd\0");
$uid = $row[0];

if($cid) {
	if($c=='save') {
		$nm1 = str_replace('"','""',$nm);
		$data1 = str_replace('"','""',$data);
		$result = db_query('update wot_chars set name="'.$nm1.'",data="'.$data1.'" where id='.$cid.' and uid='.$uid);
		die("OK\0");
	} else {
		$result = db_query('select data from wot_chars where id='.$cid.' and uid='.$uid);
		$row = mysql_fetch_array($result,MYSQL_ASSOC);
		echo $row['data'];
	}
} elseif($c=='create') {
	$nm1 = str_replace('"','""',$nm);
	$result = db_query('select id,uid from wot_chars where name="'.$nm1.'"') or die("[ERROR]srvmsg_query\0");
	if(($row=mysql_fetch_array($result,MYSQL_ASSOC)) && ($row['id'] || $row['uid']!=$uid)) die("[ERROR]srvmsg_cnmchk\0");
	$cid = $row['id'];

	$data1 = str_replace('"','""',$data);
	$ip = $_SERVER['REMOTE_ADDR'];
	$result = db_query('insert into wot_chars values(null,"'.$uid.'","'.$nm1.'","0","'.
			$ip.'","'.$data1.'","0",now(),now(),now())') or die("[ERROR]srvmsg_query\0");
	die('cid='.mysql_insert_id()."\0");
} else {
	$result = db_query('select id,name,data from wot_chars where uid='.$uid.' order by name');
	for($i=0; $row=mysql_fetch_array($result,MYSQL_ASSOC); $i++)
		echo $row['id']."\n".$row['name']."\n".$row['data']."\n\n\n";
}
echo "\0";

?>