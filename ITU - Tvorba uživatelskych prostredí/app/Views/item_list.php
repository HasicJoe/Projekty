<!DOCTYPE html>
<html lang="sk">
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="styles/item_list.css"/>
    <script src='https://kit.fontawesome.com/a076d05399.js'></script>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.0/jquery.min.js"></script>
	<script defer src="https://maps.googleapis.com/maps/api/js?key=AIzaSyAXMMazb7vGC0kcVOM3K-W2DoHaq6Wj5pQ&callback=initMap&libraries=&v=weekly"/>
	<!--mapa-->
	<script src="https://polyfill.io/v3/polyfill.min.js?features=default"></script>
	<!-------->
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta http-equiv="X-UA-Compatible" content="ie=edge" />
    <title>Zoznam zásielok</title>
</head>
<body>
<div class="nav">
<ul>
  <li><a href="/menu"><i class='fas fa-home'></i>Menu</a></li>
  <li><a class="position" href="/item_list"><i class='fas fa-boxes'></i>Nedoručené zásielky</a></li>
  <li><a href="/delivered"><i class='fas fa-clipboard-check'></i>Doručené zásielky</a></li>
  <li><a href="/insert"><i class='fas fa-truck'></i>Vložiť zásielku</a></li>
  <li><a href="/stats"><i class='fas fa-poll'></i>Štatistiky</a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"><i class='fas fa-user-alt'></i>	<?= session('vodic');?></a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"> <i class='fas fa-hashtag'></i>	<?= session('id_rozvozu');?></a></li>
</ul>
</div>
<div class="searchform" align="center">
   <input type="text" name="search" id="search" class="form-control" placeholder="Vyhľadať"/><br><br>
   <input type="checkbox" id="allDel" name="allDel"><label for="allDel">  Zobraziť všetky nedoručené položky</label>
</div>
<div id="delete_msg"></div>

<div id="filter" style="height:100px;width:80%;"></div>
	<?= session('title');?>
	<?= session('itemlist');?>

<div id="detail"> </div>
<!--MAPA-->
<div id="map_wrapper">
	<div id="map" hidden="true" style="height:400px;width:400px;border-radius:12px;"/>
</div>
	<button hidden="true" type="button" id="back_map">Skryť</button>
<!-------->

</body>
</html>

 <script>
      $(document).ready(function(){
           $('#search').keyup(function(){
                search_table($(this).val());
           });

		   $('button#back_map').click(function(e) {
				console.log("here");
				e.preventDefault();
				$('#map_wrapper').hide();
				$('#back_map').hide();
		   })

           function search_table(value){
				var finded = 0;
                $("table tr").each(function(index){
					if (index > 0) {
						var found = 'false';
						
						$(this).each(function(){
							if($(this).text().toLowerCase().indexOf(value.toLowerCase()) >= 0)
							{
								found = 'true';
							}
						});

						if(found == 'true')
						{
							finded++;	
							$(this).show();
						}
						else
						{
							$(this).hide();
						}
					}
					console.log($('table tr').length);
				
                });
				console.log(finded);
           }
      });
 </script>
 
 <!--MAPA-->
<script>
	function initMap() {
		const directionsRenderer = new google.maps.DirectionsRenderer();
		const directionsService = new google.maps.DirectionsService();
		const map = new google.maps.Map(document.getElementById("map"), {
			zoom: 15,
			center: { lat: 0, lng: 0 },
		});
		directionsRenderer.setMap(map);
		$("body").on('click', "a.show-map-btn", function(e) {
			e.preventDefault();
			$("#map").attr('hidden',false);
			$(window).scrollTop($('div#map').offset().top);
			calculateAndDisplayRoute(directionsService, directionsRenderer, $(this).attr('id'));
		});
	}

	function calculateAndDisplayRoute(directionsService, directionsRenderer, i) {
		console.log(i);
		var start = "Trenčín, Bratislavská 1804, slovakia";
		var end = document.getElementById(i).getAttribute('href');
		var request = {
			origin: start,
			destination: end,
			travelMode: 'DRIVING'
		};
		directionsService.route(request, function(result, status) {
			if (status == 'OK') {
				directionsRenderer.setDirections(result);
				console.log((result));
			} else {
				window.alert(status);
			}
		});
}
</script>
<script>
  $(document).ready(function() {
    $('body').on('click','#back_detail',function(){
     $("#detail").hide();
    });      
});
</script>
<script>
  $('body').on('click','a#agree',function(e){
  e.preventDefault();
  var href = $(this).attr('href');
  var tr_to_be_deleted = $(this).closest('tr');
  var tableIndex = parseInt(href.replace(/[^0-9.]/g, ""));
  //alert(tableIndex);
    $.ajax({
       type: "POST",
       url: "<?php echo base_url();?>/Courier/edit2",
       data: {'tableindex':tableIndex},
       success:
            function(data){
              get_id = tr_to_be_deleted.find("td:eq(0)").html();
              get_name = tr_to_be_deleted.find("td:eq(1)").html();
              tr_to_be_deleted.remove();
              $("#delete_msg").append("<p>Položka č: <b>"+get_id+", zákazníka " + get_name+ "</b> bola doručená</p>");
              $("#delete_msg").show();
            }
      });
    });
</script>
<script>
   $('body').on('click','a#details',function(e){
    e.preventDefault();
    var href = $(this).attr('href');
    var detailed_tr = $(this).closest('tr');
    var tableIndex = parseInt(href.replace(/[^0-9.]/g, ""));
      $.ajax({
         type: "GET",
         url: "<?php echo base_url();?>/Courier/detail/" + tableIndex,
         dataType: "json",
         success:
            function(data){
              console.log(data);
              $("#detail").empty();
              $("#detail").show();
              $("#detail").append(data);
              $(window).scrollTop($('#detail').offset().top);
            }
      });
    });
</script>

<script>
  $(document).ready(function(){
    window.courierDelivery = $("#searchtable").html();
  });
  
  $('#allDel').click(function(e) { 
    if ($('#allDel').is(":checked")){
     console.log("all");
      $.ajax({
         type: "GET",
         url: "<?php echo base_url();?>/Courier/displayAll",
         dataType: "json",
         success:
            function(data){
              $("#searchtable").empty();
              $("#searchtable").append(data);
            }
      });
    } else{
        $("#searchtable").empty();
        console.log(window.test)
        $("#searchtable").append(window.courierDelivery);
    }
 });
</script>