<!DOCTYPE html>
<html>
<head>
<title>Login</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0" />
<link rel="stylesheet" type="text/css" href="styles/login.css"/>
</head>
<body>
<div class="logo">
     <img src="http://xvalas.maweb.eu/upload/sds_logo-removebg-preview.png" alt="SDS">
</div>
<div class="message">
		<h1>
		 	<?= session('login_error');?>
		</h1>
</div>
<div class="form">
<h2>Prihlásenie do systému</h2>
<form name="form1" method="post">
   <input type="text" id="loginid" name="user_id" value ="" placeholder="Číslo rozvozu"><br><br>
   <input name="submit" type="submit" id="submit" value="Login"><br>
</form>
</div>
</body>
</html>
