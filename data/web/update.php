<?php

header('Content-Type: text/html; charset=utf-8'); 

if(!ini_get('register_globals')) foreach($_REQUEST as $k_123=>$v_123) ${$k_123} = $v_123;

function get_directory_tree(&$dir_array,$dir='',$root='.',$base='',$filters=array('.','..')) {
	$dirs = array_diff(scandir($root.'/'.$dir),$filters);
	foreach($dirs as $d) {
		if(is_dir($root.'/'.$dir.'/'.$d)) {
			$dir_array[] = array($root.'/'.$dir.'/'.$d,$base.'/'.$dir.'/'.$d.'/');
			get_directory_tree($dir_array,$dir.'/'.$d,$root,$base,$filters);
		} else $dir_array[] = array($root.'/'.$dir.'/'.$d,$base.'/'.$dir.'/'.$d);
	}
}

$dirs = array();
get_directory_tree($dirs,'files','.','/wotnet');
$fp = fopen('files.txt','w');
foreach($dirs as $d) {
	$s = stat($d[0]);
	$t = $s['mtime'];
	echo $t.' '.$d[1]."<br>\n";
	fwrite($fp,$t."\t".$d[1]."\n");
}
fclose($fp);

?>