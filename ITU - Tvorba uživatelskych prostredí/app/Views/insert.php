<!DOCTYPE html>
<html>
<head>
<title>Vložiť zásielku</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0" />
<meta http-equiv="X-UA-Compatible" content="ie=edge" />
<script src='https://kit.fontawesome.com/a076d05399.js'></script>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
<link rel="stylesheet" type="text/css" href="styles/item_list.css"/>
<link rel="stylesheet" type="text/css" href="styles/insert.css"/>
</head>
<script type="application/javascript">

	function focus_next(event, id) {
		$("#success_msg").html("");
		if (event.keyCode == 13) {
			event.preventDefault();
			document.getElementById(id).focus();
		}
	}

</script>

<body>
<div class="nav">
<ul>

  <li><a href="/menu"><i class='fas fa-home'></i>Menu</a></li>
  <li><a  href="/item_list"><i class='fas fa-boxes'></i>Nedoručené zásielky</a></li>
  <li><a href="/delivered"><i class='fas fa-clipboard-check'></i>Doručené zásielky</a></li>
  <li><a class="position"  href="/insert"><i class='fas fa-truck'></i>Vložiť zásielku</a></li>
  <li><a href="/stats"><i class='fas fa-poll'></i>Štatistiky</a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"><i class='fas fa-user-alt'></i>	<?= session('vodic');?></a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"> <i class='fas fa-hashtag'></i>	<?= session('id_rozvozu');?></a></li>
</ul>
</div>
<div class="insert" id="success_msg">

</div>

<div class="form">
<form method="post" id="form3">
            <label>ČÍSLO ZÁSIELKY</label>
            <input type="text" id="id_zasielky" placeholder="Vložiť číslo zásielky" name="id_zasielky" onkeydown="focus_next(event,'Vykladka');"><br>

			<label>VÝKLADKA</label>
            <input type="text" id="Vykladka" placeholder="Vložiť výkladku" name="vykladka" onkeydown="focus_next(event,'Miesto_Vykladky');"><br>


            <label>MIESTO DORUČENIA</label>
            <input type="text" id="Miesto_Vykladky" placeholder="Vložiť miesto doručenia" name="miesto" onkeydown="focus_next(event,'PSC_Vykladky');"><br>


            <label>PSČ VÝKLADKY</label>
            <input type="text" id="PSC_Vykladky" placeholder="Vložiť PSČ výkladky" name="PSC" onkeydown="focus_next(event,'Zakaznik');"><br>


            <label>ZÁKAZNÍK</label>
            <input type="text" id="Zakaznik" placeholder="Vložiť zákazníka" name="zakaznik" onkeydown="focus_next(event,'dobierka');"><br>


            <label>DOBIERKA</label>
            <input type="number" id="dobierka" placeholder="Vložiť dobierku (€)" name="dobierka" onkeydown="focus_next(event,'Colli');"><br>


            <label>POČET BALÍKOV</label>
            <input type="number" id="Colli" placeholder="Vložiť počet balíkov" name="cl" onkeydown="focus_next(event,'Mj');"><br>


            <label>MERNÁ JEDNOTKA</label>
            <input type="text" id="Mj" placeholder="Vložiť mernú jednotku" name="mj" onkeydown="focus_next(event,'Poridil');"><br>


            <label>OBJEDNAL</label>
            <input type="text" id="Poridil" placeholder="Vložiť objednávateľa" name="objednal"><br>


<input type="submit" id="submitbtn" value="PRIDAŤ ZÁSIELKU"/>

</form>

<script>
$(document).ready(function() {
	$('form#form3').on('submit', function(e) {
		e.preventDefault();
		$.ajax({
			url: "<?php echo base_url(); ?>/Courier/insert",
			method: "POST", 
			data: $('form#form3').serialize(),
			dataType: "json",
			beforeSend: function() {
				console.log(($('form#form3').serialize()));
			},	
			success: function(data) {
				if (data.error) {
					if (data.id_zasielky) 
						$('#id_zasielky').css("background","#fa8072");
					if (data.vykladka)
						$('#Vykladka').css("background","#fa8072");
					if (data.miesto)
						$('#Miesto_Vykladky').css("background","#fa8072");
					if (data.PSC)
						$('#PSC_Vykladky').css("background","#fa8072");
					if (data.zakaznik)
						$('#Zakaznik').css("background","#fa8072");
					if (data.dobierka)
						$('#dobierka').css("background","#fa8072");
					if (data.cl)
						$('#Colli').css("background","#fa8072");
					if (data.mj)
						$('#Mj').css("background","#fa8072");
					if (data.objednal)
						$('#Poridil').css("background","#fa8072");
					$("#success_msg").html("Vyplňte povinné položky");
				} else {
					//success
					document.getElementById("form3").reset();
					document.getElementById("id_zasielky").focus();
					$("#success_msg").html("Zásielka bola úspešne pridaná");
				}
			}
		});
	});
	$("input[type!='submit']").on('focus', function(e) {
		$(this).css("background", "none");
	});
});
</script>
</div>
</body>
</html>
