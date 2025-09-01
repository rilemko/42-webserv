<!DOCTYPE html>
<html lang="fr">
	<head>
		<meta charset="utf-8">
		<title>Webserv</title>
		<link rel="stylesheet" href="style.css">
	</head>
	<body>
		<header>
			<h1>WEBSERV</h1>
		</header>
		<section>
			<div class="info">
				<h2>Pages statiques</h2>
				<p class="description">Visiter des pages statiques html, comportant des images.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
						<a href="/static/fraise.html">Les fraises</a>
						<a href="/static/framboise.html">Les framboises</a>
						<a href="/static/cerise.html">Les cerises</a>
					</div>
					
				</div>
			</div>
		</section>
		<section>
			<div class="info">
				<h2>Indexation</h2>
				<p class="description">Visiter des répértoires sans index par défaut, indexant alors les fichiers présent.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
						<a href="/assets">/assets</a>
						<a href="/static">/static</a>
					</div>
				</div>
			</div>
		</section>		
		<section>
			<div class="info">
				<h2>Formulaire</h2>
				<p class="description">Envoyer un formulaire simple à php-cgi.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
						<form action="/submit" method="post">
							<input id="login" name="login" type="text" placeholder="Login">
							<input id="password" name="password" type="password" placeholder="Mot de passe">
							<input id="phone" name="phone" type="text" placeholder="Téléphone">
							<div class="row">
								<button class="secondary" type="button" onclick="this.form.reset();">Effacer</button>
								<button class="primary" type="submit">Envoyer</button>
							</div>
						</form>
					</div>
				</div>
			</div>
		</section>
		<section>
			<div class="info">
				<h2>Upload</h2>
				<p class="description">Uploader un ou plusieurs fichier(s) dans "www/upload/".</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
						<form action="/upload" method="post" enctype="multipart/form-data">
							<input id="file1" name="file1" type="file">
							<input id="file2" name="file2" type="file">
							<input id="file3" name="file3" type="file">
							<div class="row">
								<button class="secondary" type="button" onclick="this.form.reset();">Effacer</button>
								<button class="primary" type="submit">Envoyer</button>
							</div>
						</form>
					</div>
				</div>
			</div>
		</section>
		<section>
			<div class="info">
				<h2>Téléchargement</h2>
				<p class="description">Télécharger les fichier présent dans "www/upload/", ou les supprimer en envoyant un requête DELETE.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
							<table cellspacing="0">
								<?php
									foreach(array_filter(scandir("./upload", ), function($item) {
										return $item[0] !== '.';
									}) as $file)
									{
								?>
								<div class="row">
									<div class="column" style="justify-content: center;">
										<p><?= $file ?> (<span class="secondary"><?= mime_content_type("./upload/" . $file) ?></span>)</p>
									</div>
									<div class="column">
										<div class="row">
											<div class="column">
												<button class="alert" type="submit" onclick="fetch('/upload/<?= $file ?>', { method: 'DELETE'}); location.reload()">Supprimer</button>
											</div>
											<div class="column">
												<a href="upload/<?= $file ?>" download="<?= $file ?>">
													<button class="primary" type="button">Télécharger</button>
												</a>
											</div>
										</div>
									</div>
								</div>
								<?php
									}
								?>
							</table>
					</div>
				</div>
			</div>
		</section>
		<section>
			<div class="info">
				<h2>CGI</h2>
				<p class="description">Executer un script CGI.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column">
						<div class="row">
							<div class="column" style="justify-content: center;">
								<p>PHP</p>
							</div>
							<div class="column">
								<form action="script.php?getkey1=getval1&getkey2=getval2" method="post">
									<input id="postkey1" name="postkey1" type="text" value="postval1" hidden>
									<input id="postkey2" name="postkey2" type="text" value="postval2" hidden>
									<button class="primary" type="submit">Executer</button>
								</form>
							</div>
						</div>
						<div class="row">
							<div class="column"  style="justify-content: center;">
								<p>Python</p>
							</div>
							<div class="column">
								<form action="script.py?getkey1=getval1&getkey2=getval2" method="post">
									<input id="postkey1" name="postkey1" type="text" value="postval1" hidden>
									<input id="postkey2" name="postkey2" type="text" value="postval2" hidden>
									<button class="primary" type="submit">Executer</button>
								</form>
							</div>
						</div>
					</div>
				</div>
			</div>
		</section>
		<section>
			<div class="info">
				<h2>Cookies/Session</h2>
				<p class="description">Vérifier la persistance de la session ou supprimer le cookie correspondant pour le renouveler.</p>
			</div>
			<div class="section">
				<div class="card">
					<div class="column" style="justify-content: center;">
						<p>Votre identifiant de session est:</p>
						<b><?= isset($_COOKIE["sessid"]) ? $_COOKIE["sessid"] : "Undefined" ?></b>
					</div>
					<div class="column">
						<button class="alert" type="button" onclick="document.cookie = 'sessid=; expires=Thu, 01 Jan 1970 00:00:00 GMT'; location.reload()">Supprimer</button>
					</div>
				</div>
			</div>
		</section>
	</body>
</html>
