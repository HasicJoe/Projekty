<!DOCTYPE html>
<html lang="sk">
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="http://itutesting.hys.cz/styles/item_list.css"/>
    <script src='https://kit.fontawesome.com/a076d05399.js'></script>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.0/jquery.min.js"></script>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Doručené zásielky</title>
</head>
<body>

<div class="nav">
<ul>

  <li><a href="/menu"><i class='fas fa-home'></i>Menu</a></li>
  <li><a  href="/item_list"><i class='fas fa-boxes'></i>Nedoručené zásielky</a></li>
  <li><a class="position" href="/delivered"><i class='fas fa-clipboard-check'></i>Doručené zásielky</a></li>
  <li><a href="/insert"><i class='fas fa-truck'></i>Vložiť zásielku</a></li>
  <li><a href="/stats"><i class='fas fa-poll'></i>Štatistiky</a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"><i class='fas fa-user-alt'></i>	<?= session('vodic');?></a></li>
  <li id="userinfo" style="float:right"><a class="info" href="#user"> <i class='fas fa-hashtag'></i>	<?= session('id_rozvozu');?></a></li>
</ul>
</div>


<div class="searchform" align="center">
	   <input type="text" name="search" id="search" class="form-control" placeholder="Vyhľadať"/>
</div>
<?= session('title');?>
<?= session('doruc');?>
<div id="count"></div>
<div id="detail"> </div>
</body>
</html>


 <script>
      $(document).ready(function(){
           $('#search').keyup(function(){
                search_table($(this).val());
           });
           function search_table(value){
                $('table tr').each(function(index){
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
							$(this).show();
						}
						else
						{
							$(this).hide();
						}
					}
                });
           }
      });
 </script>
 
 <script>
  $(document).ready(function() {
    $('body').on('click','#back_detail',function(){
     $("#detail").hide();
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
            //  console.log(data);
              $("#detail").empty();
               $("#detail").show();
              $("#detail").append(data);
              $(window).scrollTop($('#detail').offset().top);
              //$(window).$("#detail").scrollTop(0);
            }
      });
    });
</script>
