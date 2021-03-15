<?php namespace App\Controllers;

use App\Models\CourierModel;

class Courier extends BaseController{

	public function itemlist() {
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		}
		else {
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$doru = 0;
			$getQuery = $courier->getCourierDelivery($session->id_rozvozu,$doru);
			$title =" <table id='searchtable'><thead>
								<th> Číslo zásielky </th>
								<th> Výkladka</th>
								<th> Miesto Výkladky</th>
								<th> PSČ výkladky</th>
								<th> Dobierka </th>
								<th> Telefónne Číslo </th>
								<!--MAPA-->
								<th> Trasa</th>
								<!-------->
								<th> Detaily </th>
								<th> Doručené </th>
								</thead>";
			$result = "";
			$test["phone"]="0908235125";
			

			foreach ($getQuery->getResultArray() as $row){
				///MAPA
				$map = $row['Vykladka'] . ', ' . $row['Miesto_Vykladky'] . ', ' . $row['PSC_Vykladky']. ', slovakia';
				/////////
				$result = $result . '<tr id=\'tablebody\'><td id=\'numb\'>'. $row['id_zasielky'] .'</td>
					<td>'. $row['Vykladka'] .'</td>
					<td>'. $row['Miesto_Vykladky'] .'</td>
					<td>'. $row['PSC_Vykladky'] .'</td>
					<td>'. $row['Dobierka'] . ' €</td>
					<td><a href=\'tel:' . $test['phone'] . '\' target=\'_blank\' class=\'btn btn-table\' id=\'tablebtn\' >' .$test['phone']. '</a> </td>
					<!--MAPA-->
					<td><a href=\''. $map. '\' class=\'btn btn-table show-map-btn\' id=\'showmap'.$row['id'].'\' >Zobraziť</a></td>  
					<!-------->
					<td><a href=/detail/'.$row['id'].' class=\'btn btn-table\' id=\'details\'>Detaily</a></td><td><a href=edit/'.$row['id'].' class=\'btn btn-table\' id=\'agree\'>Potvrdiť</a></td>
					</tr>';
			}
			$session->title = $title;
			$session->itemlist = $result;
			return  view('item_list');
		}
	}


	public function displayAll() {
			$courier = new CourierModel;
			$getQuery = $courier->getAllDelivery();
			$result = "";
			$result = $result . " <table id='searchtable'><thead>
								<th> Číslo zásielky </th>
								<th> Výkladka</th>
								<th> Miesto Výkladky</th>
								<th> PSČ výkladky</th>
								<th> Dobierka </th>
								<th> Telefónne Číslo </th>
								<!--MAPA-->
								<th>Trasa</th>
								<!-------->
								<th> Detaily </th>
								<th> Doručené </th>
								</thead>";
			$test["phone"]="0908235125";
			foreach ($getQuery->getResultArray() as $row){
				//MAPA
				$map = $row['Vykladka'] . ', ' . $row['Miesto_Vykladky'] . ', ' . $row['PSC_Vykladky']. ', slovakia';
				///////
				$result = $result . '<tr id=\'tablebody\'><td id=\'numb\'>'. $row['id_zasielky'] .'</td>
					<td>'. $row['Vykladka'] .'</td>
					<td>'. $row['Miesto_Vykladky'] .'</td>
					<td>'. $row['PSC_Vykladky'] .'</td>
					<td>'. $row['Dobierka'] . ' €</td>
					<td><a href=\'tel:' . $test['phone'] . '\' target=\'_blank\' class=\'btn btn-table\' id=\'tablebtn\' >' .$test['phone']. '</a> </td>
					<!--MAPA-->
					<td><a href=\''. $map. '\' class=\'btn btn-table show-map-btn\' id=\'showmap'.$row['id'].'\' >Zobraziť</a></td>  
					<!-------->
					<td><a href=/detail/'.$row['id'].' class=\'btn btn-table\' id=\'details\'>Detaily</a></td><td><a href=edit/'.$row['id'].' class=\'btn btn-table\' id=\'agree\'>Potvrdiť</a></td>
					</tr>';
			}
			echo json_encode($result);
	}



public function delivered() {

		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		} else {
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$doru = 1;
			$getQuery = $courier->getCourierDelivery($session->id_rozvozu,$doru);
			$title =" <table id='searchtable'><thead>
					<th> Číslo zásielky </th>
					<th> Výkladka</th>
					<th> Miesto Výkladky</th>
					<th> PSČ výkladky</th>
					<th> Dobierka </th>
					<th> Telefónne Číslo </th>
					<th> Detaily </th>
					</thead>";
			$result = "";
			$test["phone"]="0908235125";

			foreach ($getQuery->getResultArray() as $row) {
			$result = $result . '<tr class=\'tablebody\'><td>'. $row['id_zasielky'] .'</td>
			  <td>'. $row['Vykladka'] .'</td>
			  <td>'. $row['Miesto_Vykladky'] .'</td>
				<td>'. $row['PSC_Vykladky'] .'</td>
				<td>'. $row['Dobierka'] . ' €</td>
				<td><a href=\'tel:' . $test['phone'] . '\' target=\'_blank\' class=\'btn btn-table\' id=\'tablebtn\' >' .$test['phone']. '</a> </td>
				<td><a href=/detail/'.$row['id'].' class=\'btn btn-table\' id=\'details\'>Detaily</a></td>
				</tr>';
			}
			$session->title = $title;
			$session->doruc = $result;
			return  view('delivered');
		}
	}


    public function stats() {
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		} else {
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$getQuery = $courier->getUser($session->id_rozvozu);
			$pocet_poloziek = 0;
			$dobierka = 0;
			$suma_pocet_poloziek = 0;
			$suma_dobierka = 0;

			foreach ($getQuery->getResultArray() as $row) {
				if($row['dorucene'] != 0 ) {
					$pocet_poloziek= $pocet_poloziek+1;
					$dobierka+=$row['Dobierka'];
				}
					$suma_pocet_poloziek+=1;
					$suma_dobierka+=$row['Dobierka'];
			}
			$session->celkovodob = $dobierka;
			$session->polozky = $pocet_poloziek;
			$session->suma = $suma_pocet_poloziek;
			$session->celk_suma = $suma_dobierka;
			return view('stats');
		}
	}

	public function statsDriver(){
		$courier = new CourierModel;
		$session = \Config\Services::session();
		$user_name = $session->origVodic;
		$getQuery = $courier->getByName($user_name);
		$pocet_poloziek = 0;
		$dobierka = 0;
		$suma_pocet_poloziek = 0;
		$suma_dobierka = 0;
		foreach ($getQuery->getResultArray() as $row) {
			if($row['dorucene'] != 0 ) {
				$pocet_poloziek= $pocet_poloziek+1;
				$dobierka+=$row['Dobierka'];
			}
				$suma_pocet_poloziek+=1;
				$suma_dobierka+=$row['Dobierka'];
		}
		$data = array(
		'test' => $session->origVodic,	
		'dobierka' => $dobierka,
		'pocet_poloziek' => $pocet_poloziek,
		'suma_dobierka' => $suma_dobierka,
		'suma_pocet_poloziek' => $suma_pocet_poloziek,
		);
		echo json_encode($data);
	}

	public function getStats() {
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		} else {
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$getQuery = $courier->getAll($session->id_rozvozu);
			$pocet_poloziek = 0;
			$dobierka = 0;
			$suma_pocet_poloziek = 0;
			$suma_dobierka = 0;

			foreach ($getQuery->getResultArray() as $row) {
				if($row['dorucene'] != 0 ) {
					$pocet_poloziek= $pocet_poloziek+1;
					$dobierka+=$row['Dobierka'];
				}
					$suma_pocet_poloziek+=1;
					$suma_dobierka+=$row['Dobierka'];
			}
			$datas = array(
			'dobierka' => $dobierka,
			'pocet_poloziek' => $pocet_poloziek,
			'suma_dobierka' => $suma_dobierka,
			'suma_pocet_poloziek' => $suma_pocet_poloziek,
			);
			echo json_encode($datas);
		}
	}

	public function edit($dorucene) {
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		}else{
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$query = $courier->setAsDelivered($dorucene);
		}
	}
	public function edit2() {
		$request = service('request');
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		}else{
			$courier = new CourierModel;
			$session = \Config\Services::session();
			$dorucene = $request->getPost('tableindex');
			$query = $courier->setAsDelivered($dorucene);
		}
	}

	public function detail($id) {
		$request = service('request');
		//$item_id = $request->get('tableindex');
		//echo json_encode($item_id);
		$courier = new CourierModel;
		$session = \Config\Services::session();
		$session->asdf = $id;
		$getQuery = $courier->showItemDetail($id);
		$item_detail = "";
		foreach ($getQuery->getResultArray() as $row) {
			$item_detail ='
			<h2>Detail položky '. $row['id_zasielky'] .'</h2>
			<table>
				<tr><td id=\'nadpis\'>Zákazník</td> <td>'. $row['Zakaznik'] .'</td></tr>
				<tr><td id=\'nadpis\'>Číslo zásielky</td> <td>'. $row['id_zasielky'] .'</td>
				<tr><td id=\'nadpis\'>Výkladka</td> <td>'. $row['Vykladka'] .'</td>
				<tr><td id=\'nadpis\'>Miesto výkladky</td> <td>'. $row['Miesto_Vykladky'] .'</td>
				<tr><td id=\'nadpis\'>PSČ Výkladky</td> <td>'. $row['PSC_Vykladky'] .'</td>
				<tr><td id=\'nadpis\'>Dobierka</td> <td>'. $row['Dobierka'] .'€</td>
				<tr><td id=\'nadpis\'>Tuzemský Dor.</td> <td>'. $row['Tuz_Dor'] .'</td>
				<tr><td id=\'nadpis\'>Vodič</td> <td>'. $row['Vodic'] .'</td>
				<tr><td id=\'nadpis\'>Colli</td> <td>'. $row['Colli'] .'</td>
				<tr><td id=\'nadpis\'>Merná jednotka</td> <td>'. $row['Mj'] .'</td>
				<tr><td id=\'nadpis\'>Hmotnosť</td> <td>'. $row['Hmotnost'] .'</td>
				<tr><td id=\'nadpis\'>SPZ</td> <td>'. $row['SPZ'] .'</td>
				<tr><td id=\'nadpis\'>Objednal</td> <td>'. $row['Poridil'] .'</td>
				<tr><td id=\'nadpis\'>Telefónne Číslo</td> <td> UNDEFINED </td></tr>
				</table>
				<div style=\'text-align: center; margin:40px;\'>
				<button type=\'button\' id=\'back_detail\'>Skryť Detaily položky</button>';
		}
		echo json_encode($item_detail);
		
	}

	public function form() {
		if(session('prihlaseny')  === NULL) {
			return redirect()->to('/login');
		}
		$session = \Config\Services::session();
		$session->invalid = "";
		$session->done = "";
		return view('insert');
	}

	public function insert(){
		$request =		\Config\Services::request();
		$session =		\Config\Services::session();
		$validation =   \Config\Services::validation();
		$courier =		new CourierModel;

		$session->done ="";
		$session->invalid="";
		helper(['form','url']);

		$checkSubmit = $this->validate([
			'id_zasielky' => 'required',
			'vykladka' => 'required',
			'miesto' => 'required',
			'PSC' => 'required',
			'zakaznik' => 'required',
			'dobierka' => 'required',
			'cl' => 'required|numeric',
			'mj' => 'required',
			'objednal' => 'required'
			]);
														
		if ($checkSubmit) {
			$id_zasielky =	$request->getPost('id_zasielky');
			$vykladka =		$request->getPost('vykladka');
			$miesto =		$request->getPost('miesto');
			$PSC =			$request->getPost('PSC');
			$zakaznik =		$request->getPost('zakaznik');
			$dobierka =		$request->getPost('dobierka');
			$cl =			$request->getPost('cl');
			$mj =			$request->getPost('mj');
			$objednal =		$request->getPost('objednal');
			$vodic =		$session->get('vodic');
			$id_zvozu =		$session->get('id_rozvozu');
			$spz =			$session->get('SPZ');
			
			$result = $courier->insertNew($id_zvozu,$spz,$id_zasielky,$vykladka,$miesto,$PSC,$zakaznik,$vodic,$dobierka,$cl,$mj,$objednal);

			$res = array(
				'error' => false
			);

		} else {
			$res = array(
				'error' => true,
				'id_zasielky' =>  $request->getPost('id_zasielky') === "",
				'vykladka' =>  $request->getPost('vykladka') === "",
				'miesto' =>  $request->getPost('miesto') === "",
				'PSC' =>  $request->getPost('PSC') === "",
				'zakaznik' =>  $request->getPost('zakaznik') === "",
				'dobierka' =>  $request->getPost('dobierka') === "",
				'cl' =>  $request->getPost('cl') === "",
				'mj' =>  $request->getPost('mj') === "",
				'objednal' =>  $request->getPost('objednal') === "",
				'vodic' =>  $request->getPost('vodic') === "",
				'id_rozvozu' =>  $request->getPost('id_rozvozu') === "",
				'SPZ' =>  $request->getPost('SPZ') === "",
			);
		}

		echo json_encode($res);
		return;
	}
}
