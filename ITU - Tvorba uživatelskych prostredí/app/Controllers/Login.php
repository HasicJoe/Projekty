<?php namespace App\Controllers;

use App\Models\CourierModel;


class Login extends BaseController {

    public function index() {
        if(session('prihlaseny')  !== NULL){
            view('/menu');
            return redirect()->to('/menu');
        }else{
            return view('/index');
        }
    }

    public function login(){
        if(session('prihlaseny')  !== NULL){
            view('menu');
            return redirect()->to('/menu');
        }else {
            $session = \Config\Services::session();
            $session->login_error ="";
            return view('login');
        }
    }

    public function logForm() {

        $request = \Config\Services::request();
        $courier = new CourierModel;
	    $session = \Config\Services::session();
        helper(['form','url']);
        $checkSubmit = $this->validate(['user_id' => 'required']);
				if(!$checkSubmit){
					$session->login_error = "Nevyplnený login";
					return view('login');
				}

        $user_id = $request->getPost('user_id');
        $result = $courier->getUser($user_id);
        $row = $result->getFirstRow();

        if(isset($row)) {
            $session->id_rozvozu = $row->id_zvozu;
            $session->SPZ = $row->SPZ;
            $session->prihlaseny = true;
            $vodic = $row->Vodic;
            $session->origVodic = $row->Vodic;
            $brackets = array("(",")");
            $readytoset = str_replace($brackets,"",$vodic);
            $session->vodic = $readytoset;
            view('/menu');
            return redirect()->to('/menu');
        }else {
            
            $session->login_error = "Pre zadaný vstup  $user_id neexistuje číslo rozvozu";
            return view('login');
        }
    }

	public function menu() {
        if(session('prihlaseny')  === NULL) {
            return redirect()->to('/login');
        } else {
            return view('menu');
        }
	}

    public function logout() {
        $session = \Config\Services::session();
        $session->destroy();
        view('index');
        return redirect()->to('/index');
    }
}
