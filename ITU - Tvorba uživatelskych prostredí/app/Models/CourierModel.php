<?php namespace App\Models;

use CodeIgniter\Model;


class CourierModel extends Model {

    protected $table = 'test';
    protected $primaryKey = 'id';
    protected $allowedFields = ['id_zvozu','Datum_vykladky','id_zasielky','Tuz_zasiel','Tuz_Dor','Vykladka','Miesto_Vykladky','PSC_Vykladky','Zakaznik','Vodic','Dobierka','Colli','Mj','Hmotnost','SPZ','Poridil','dorucene'];

    public function getUser($user_id) {
        $db = \Config\Database::connect();
        $builder = $db->table('test');
        $builder->select('*');
        $builder->where('id_zvozu',$user_id);
        $query = $builder->get();
        return $query;
    }

    public function getByName($user_name) {
        $db = \Config\Database::connect();
        $builder = $db->table('test');
        $builder->select('*');
        $builder->where('Vodic',$user_name);
        $query = $builder->get();
        return $query;
    }

    public function getAll() {
        $db = \Config\Database::connect();
        $builder = $db->table('test');
        $builder->select('*');
        $query = $builder->get();
        return $query;
    }

    public function getCourierDelivery($user_id,$doruc) {
        $db = \Config\Database::connect();
        $diff = $doruc;
        $builder = $db->table('test');
        $builder->select('*');
        $builder->where('id_zvozu',$user_id);
        $builder->where('dorucene',$diff);
        $query = $builder->get();
        return $query;
    }

    public function getAllDelivery(){
       $db = \Config\Database::connect();
       $builder = $db->table('test');
       $builder->select('*');
       $query = $builder->get();
       return $query;
    }
    
    public function showItemDetail($item_id){
        $db = \Config\Database::connect();
        $builder = $db->table('test');
        $builder->select('*');
        $builder->where('id',$item_id);
        $query = $builder->get();
        return $query;
    }


    public function setAsDelivered($user_id){
        $db = \Config\Database::connect();
        $builder = $db->table('test');
        $builder->set('dorucene',1);
        $builder->where('id',$user_id);
        $builder->update();
        return true;
    }

    public function insertNew($id_zvozu,$spz,$id_zasielky,$vykladka,$miesto,$PSC,$zakaznik,$vodic,$dobierka,$cl,$mj,$objednal){
      $db = \Config\Database::connect();
      $builder = $db->table('test');
      $data = [
              'id_zvozu'        => $id_zvozu,
              'id_zasielky'     => $id_zasielky,
              'Vykladka'        => $vykladka,
              'Miesto_vykladky' => $miesto,
              'PSC_Vykladky'    => $PSC,
              'Zakaznik'        => $zakaznik,
              'Vodic'           => $vodic,
              'Dobierka'        => $dobierka,
              'Colli'           => $cl,
              'MJ'              => $mj,
              'SPZ'             => $spz,
              'Poridil'         => $objednal
              ];
      $builder->insert($data);
      return $builder;
    }
}
