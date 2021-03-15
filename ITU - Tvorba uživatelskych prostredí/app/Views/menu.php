<!DOCTYPE html>
<html>
<head>
<title>ITU 2020</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0" />
<link rel="stylesheet" type="text/css" href="styles/menu.css"  />
<script src='https://kit.fontawesome.com/a076d05399.js'></script>
</head>
<body>
	<div class="content">
		<div class="menu">

			<div id="info">
		       	 <p class="log"><i class='fas fa-user-alt'></i>
		        	<?= session('vodic');?>
         </p>
		         	<p class="log"><i class='fas fa-hashtag'></i>
		        	<?= session('id_rozvozu');?>
             </p>
	   		</div>
	   		<div class="find">
	   		    <a href="/item_list">
				<div class="item">
					<div class="favic">
						<i class='fas fa-boxes'></i>
					</div>
					Zobraziť nedoručené zásielky
				</div>
				</a>
				<a href="/delivered">
				<div class="item">
					<div class="favic">
						<i class='fas fa-clipboard-check'></i>
					</div>
					Zobraziť doručené zásielky
				</div>
				</a>
				<a href="/insert">
				<div class="item">
					<div class="favic">
						<i class='fas fa-truck'></i>
					</div>
					Vložiť novú zásielku
				</div>
				</a>
				<a href="/stats">
				<div class="item">
					<div class="favic">
						<i class='fas fa-poll'></i>
					</div>
					Zobraziť štatistiky
				</div>
				</a>
			</div>
			<div class="logout">
			    <a href="/logout" class="btn btn-danger">
				<p align="center">
					<i class='fas fa-sign-out'></i>
		            Odhlásiť sa
	    		</p>
	    		</a>
			</div>
		</div>
	</div>
</body>
