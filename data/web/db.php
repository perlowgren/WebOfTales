<?php


require 'lib.php';


db_open();

if(!$db) $db = 'wot.db';

?><html>
<head>
</head>
<body>
<form name="form" action="db.php" method="post">
<input type="text" name="db" value="<?= $db ?>" style="width:100%;">
<textarea name="q" style="width:100%;height:200;"><?= $q ?></textarea><br>
<input type="submit">
</form>
<?php

if($q) {
	echo "<table>\n";
	if($result=db_query($q)) {
		echo "<tr><td>".$q."</td></tr>\n";
		if($result!==TRUE && $result!==FALSE) {
			$i = 0;
			$n = mysql_num_fields($result);
			if($n>0) {
				echo "<tr><td><table border=1>\n";
				echo "<tr><td></td>\n";
				while($i<$n) {
					$field = mysql_field_name($result,$i);
					echo "<td><b>".$field."</b></td>\n";
					$i++;
				}
				echo "</tr>\n";
				$i = 0;
				while($row=mysql_fetch_array($result,MYSQL_NUM)) {
					echo "<tr><td>$i</td>\n";
					$j = 0;
					while($j<$n) echo "<td>".$row[$j++]."</td>\n";
					echo "</tr>\n";
					$i++;
				}
				echo "</table></td></tr>\n";
			}
		}
	}
	else echo "<tr><td><p>Förfrågan misslyckades!<br>\n$q</p></td></tr>\n";
	echo "</table>\n";
}

?>
</body>
</html>
