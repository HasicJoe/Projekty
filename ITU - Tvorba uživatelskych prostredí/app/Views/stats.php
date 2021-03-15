<!DOCTYPE html>
<html>
<head>
<title>Štatistiky</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0" />
<link rel="stylesheet" type="text/css" href="http://itutesting.hys.cz/styles/login.css""  />
<link rel="stylesheet" type="text/css" href="http://itutesting.hys.cz/styles/item_list.css"/>
<script src='https://kit.fontawesome.com/a076d05399.js'></script>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
</head>
<body>
<div class="nav">
<ul>
  <li><a href="/menu"><i class='fas fa-home'></i>Menu</a></li>
  <li><a href="/item_list"><i class='fas fa-boxes'></i>Nedoručené zásielky</a></li>
  <li><a href="/delivered"><i class='fas fa-clipboard-check'></i>Doručené zásielky</a></li>
  <li><a href="/insert"><i class='fas fa-truck'></i>Vložiť zásielku</a></li>
  <li><a class="position" href="/stats"><i class='fas fa-poll'></i>Štatistiky</a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"><i class='fas fa-user-alt'></i><?= session('vodic');?> </a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"> <i class='fas fa-hashtag'></i><?= session('id_rozvozu');?></a></li>
</ul>
</div>
<div class="stats">
<h2>Výkon dnes - <?= session('vodic');?></h2>

 <h3 id="TEST">Číslo rozvozu: <?= session('id_rozvozu');?> </h3>
 <h3>Vozidlo: <?= session('SPZ');?> </h3>
 <h3>Počet stopov: <?= session('suma');?> </h3>
 <h3>Doručené stopy:  <?= session('polozky');?> </h3>
 <h3>Nedoručené stopy: <?= (session('suma') -session('polozky')) ;?> </h3>
 <h3>Dobierka (doručené) : <?= session('celkovodob');?> € </h3>
 <h3>Celková dobierka ( Σ ) : <?= session('celk_suma');?> € </h3>
 <input id="sds" type="button" value="Zobraziť celkové štatistiky" style="padding:10px; margin:10px;">
 <input id="sds2" type="button" value="Zobraziť osobné štatistiky" style="padding:10px; margin:10px;">
</div>
<div id="new">
  
</div>
</body>

<script>
$(document).ready(function(){
    $("#sds").click(function()
    {
     $.ajax({
         type: "GET",
         url: "<?php echo base_url();?>/Courier/getStats",
         dataType: "json",
         success:
              function(data){
                 $("#new").empty();
                 $("#new").show();
                 $("#new").append("<h2>Štatistiky Depo</h2>");
                 $("#new").append("<h3>Doručená dobierka: "+ parseFloat(data.dobierka).toFixed() +"€</h3>");
                 $("#new").append("<h3>Počet doručených položiek: "+ data.pocet_poloziek +"</h3>");
                 $("#new").append("<h3>Celková dobierka: "+ parseFloat(data.suma_dobierka).toFixed() +"€</h3>");
                 $("#new").append("<h3>Celkový počet položiek: "+ data.suma_pocet_poloziek +"</h3>");
                 $(window).scrollTop($('#new').offset().top);
              }
          });
    });
 });
 </script>

 <script>
$(document).ready(function(){
    $("#sds2").click(function()
    {
      var get_data = $("h3#TEST").text();
      var id = parseInt(get_data.replace(/[^0-9.]/g, ""));
     $.ajax({
         type: "GET",
         url: "<?php echo base_url();?>/Courier/statsDriver",
         dataType: "json",
         success:
              function(data){
                console.log(data);
                 $("#new").empty();
                 $("#new").empty();
                 $("#new").append("<h2>Celkové štatistiky vodiča</h2>");
                 $("#new").append("<h3>Doručená dobierka: "+ parseFloat(data.dobierka).toFixed() +"€</h3>");
                 $("#new").append("<h3>Počet doručených položiek: "+ data.pocet_poloziek +"</h3>");
                 $("#new").append("<h3>Celková dobierka: "+ parseFloat(data.suma_dobierka).toFixed() +"€</h3>");
                 $("#new").append("<h3>Celkový počet položiek: "+ data.suma_pocet_poloziek +"</h3>");
                 $(window).scrollTop($('#new').offset().top);
              }
          });
     return false;
    });
 });
 </script>