<!-- pour lancer le site utiliser la commande: -->
<!-- python3 -m http.server -->
<!-- puis se connecter a http://localhost:8000/ -->

<!DOCTYPE html>
<html lang="fr">
	<head>
		<meta charset="utf-8">
		<title>Les fruits rouges</title>
	</head>
	<body>
		<h1>SUB</h1>
		<?php
		echo '<pre>';
		var_export($_SERVER);
		echo '</pre>';
		echo phpinfo();
		?>
	</body>
</html>
