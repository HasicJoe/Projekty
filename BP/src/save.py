"""
Autor: Samuel Valaštín
Dátum : 21.2.2022
Súbor funkcii pre komunikáciu s google drive API.
    Tento zdrojový súbor obsahuje:
        Autentizaciu pre google drive API
        načítanie uloženého rámca vo formáte .pickle
        načítanie metadát a štatistík vo formáte .json
        ukladanie spracovaných rámcov a štatistík
        zmazanie dočasne uložených rámcov a štatistík
        moznosti pre pracu s google drivom medzi ktore patri:
            vytvorenie priecinku
            opatovne prihlasenie uzivatela
            listovanie dostupnych priecinkov
            listovanie suborov v aktivnom priecinku
            ....
"""

import os, json, pickle
from datetime import datetime
from flask import Blueprint, request, jsonify
from pydrive2.auth import GoogleAuth
from pydrive2.drive import GoogleDrive
import pandas as pd

save = Blueprint("save", __name__)


class g_Drive:
    """
    Trieda pre komunikáciu a autentizáciu s Google Drive API.
    """
    def __init__(self):
        self.load_token()
        self.file_loc = 'test.pkl'
        self.gauth = GoogleAuth()
        self.gauth.LoadCredentialsFile("token.json")
        if self.gauth.credentials is None:
            self.authenticate()
        elif self.gauth.access_token_expired:
            try:
                self.gauth.Refresh()
            except:
                if os.path.exists('token.json'):
                    os.remove('token.json')
                self.authenticate()
        else:
            self.gauth.Authorize()
        self.gauth.SaveCredentialsFile('token.json')
        self.drive = GoogleDrive(self.gauth)


    def authenticate(self):
        """
        Autentifikacia pre google shared drive.
        """
        self.gauth.GetFlow()
        self.gauth.flow.params.update({"access_type": "offline"})
        self.gauth.flow.params.update({"approval_prompt": "force"})
        """
        pre moznost autentifikacie na hostingu =: aktualne nefunguje pre
        neplatenu verziu hosting neumoznuje specifikaciu portov a runtime 
        porty vybera nahodne
        """
        if 'HEROKU' in os.environ:
            self.gauth.LocalWebserverAuth(host_name='logmine.herokuapp.com')
        else:
            self.gauth.LocalWebserverAuth()


    def get_drive(self):
        """
        Vratenie drive objektu.
        """
        return self.drive


    def get_directory_token(self):
        """
        Ziskanie identifikatoru pre pouzivany google priecinok.
        """
        return self.folder_token


    def load_token(self):
        """
        Nacitanie aktualneho identifikatoru priecinku na g drive.
        """
        try:
            with open('folder_token.txt', 'r') as f_token:
                self.folder_token = f_token.read().rstrip()
        except:
            self.folder_token = None


    def get_dataframe(self, frame_filename):
        """
        Docasne ziskanie a ulozenie spracovaneho datoveho ramca a jeho nasledne nacitanie do pandas dataframe.
        """
        file_list = self.drive.ListFile({"q": f"'{self.folder_token}' in parents and trashed=false"}).GetList()
        for file in file_list:
            if file["originalFilename"] == frame_filename:
                f = self.drive.CreateFile({'id' : file['id']})
                f.GetContentFile(self.file_loc)
                df = pd.read_pickle(self.file_loc)
                os.remove(self.file_loc)
                return df


    def delete_files(self, filenames):
        """
        Zmazanie suborov ulozenych na google disku.
        """
        file_list = self.drive.ListFile({"q": f"'{self.folder_token}' in parents and trashed=false"}).GetList()
        for file in file_list:
            if file['originalFilename'] in filenames:
                try:
                    f = self.drive.CreateFile({'id' : file['id']})
                    f.Delete()
                except:
                    return False
        return True


    def find_files(self):
        """
        Najdenie suborov pre aktualne vyuzivany google priecinok.
        """
        file_data = list()
        file_data.append(
            pack_table_row(
                ['Cesta', 'Dátum vytvorenia', 'Mime typ'], 
                header=True, 
                caption='Zoznam dostupných súborov'
            )
        )
        try: 
            file_list = self.drive.ListFile({"q": f"'{self.folder_token}' in parents and trashed=false"}).GetList()
            for file in file_list:
                file_data.append(pack_table_row([file['originalFilename'], file['createdDate'], file['mimeType']]))
        except:
            file_data.append(pack_table_row(['Chyba', 'Chyba', 'Chyba']))
            file_data = [file_data[0], file_data[-1]]
        return ''.join(file_data)


    def find_folders(self):
        """
        Najdenie dostupnych priecinkov na google drive.
        """
        folders = list()
        folders.append(pack_table_row(['Názov priečinku', 'Dátum a čas vytvorenia', 'Identifikátor priečinku'], header=True, caption='Zoznam dostupných priečinkov'))
        try:
            folder_list = self.drive.ListFile({'q': "'root' in parents and trashed=false and mimeType='application/vnd.google-apps.folder'"}).GetList()
            for folder in folder_list:
                folders.append(pack_table_row([folder['title'], folder['createdDate'], folder['id']]))
        except:
            folders.append(['Chyba', 'Chyba', 'Chyba'])
            folders = [folders[0], folders[-1]]
        return ''.join(folders)


    def create_folder(self, foldername):
        """
        Vytvorenie noveho pricink na vzdilenom google disku.
        """
        try:
            foldernames, root_id = self.get_folder_names_and_parrent_ids()
            if foldername in foldernames:
                return False
            root_parent = dict(id=root_id)
            folder = self.drive.CreateFile({
                'title' : foldername,
                'parents' : root_parent,
                'mimeType' : 'application/vnd.google-apps.folder'
            })
            folder.Upload()
        except:
            return False
        return True


    def get_folder_names_and_parrent_ids(self):
        """
        Ziskanie mien vytvorenych priecinkov, pre kontrolu kolizie pri vytvoreni noveho priecinku.
        Funkcia zaroven poskytuje ziskanie korenoveho identifikatoru pre google disk. 
        """
        try:
            folder_list = self.drive.ListFile({'q': "'root' in parents and trashed=false and mimeType='application/vnd.google-apps.folder'"}).GetList()
            folder_names = [folder['title'] for folder in folder_list]
            parrent_ids = [folder['parents'][0]['id'] if len(folder['parents']) == 1 else False for folder in folder_list]
        except:
            return False
        parrent_id = list(set(parrent_ids))
        if len(parrent_id) > 1:
            return False
        return folder_names, str(parrent_id)
    
            
def pack_table_row(columns, header=False, caption=''):
    """
    Spracovanie dat do podoby riadku HTML tabulky.
    """
    if header:
        table_cols = [f"<th>{col}</th>" for col in columns]
    else:
        table_cols = [f"<td>{col}</td>" for col in columns]
    row = f"<tr>{''.join(table_cols)}</tr>"
    if header:
        row = f"<caption>{caption}</caption><thead class='table-dark'>{row}</thead>"
    return row
    
        
@save.route("/delete_processed_log", methods=["GET", "POST"])
def delete_processed_log():
    """
    Zmazanie dočasne uložených súborov po uživateľskej odozve.
    """
    if not request.args["metadata"] or not request.method == "POST":
        return jsonify({"error": "Nemožno zmazať metadata.", "status": 203}), 203
    else:
        metadata = request.args.get("metadata")
        dataframe = metadata[:-5] + ".pkl"
        try:
            os.remove(metadata)
            os.remove(dataframe)
        except:
            return jsonify({"error": "Nepodarilo sa zmazať metadata.", "status": 204}), 204
        return jsonify({"ok": "Metadáta a dátový rámec zmazaný.", "status": 200}), 200


@save.route("/save_to_drive", methods=["POST"])
def save_to_drive():
    """
    Uloženie spracovaného rámca ciest a štatistík na Google Drive.
    """
    if not request.args["metadata"] or not request.method == "POST":
        return jsonify({"error": "Internal error. Nemožno uložiť spracovaný log.", "status": 205}), 205
    else:
        try:
            drive = g_Drive()
            folder_token = drive.get_directory_token()
            drive_connection = drive.get_drive()
            metadata_filename = request.args.get("metadata")
            processed_frame_filename = metadata_filename[:-5] + ".pkl"
            metadata = drive_connection.CreateFile({"parents": [{"id": f"{folder_token}"}]})
            log = drive_connection.CreateFile({"parents": [{"id": f"{folder_token}"}]})
            metadata.SetContentFile(metadata_filename)
            log.SetContentFile(processed_frame_filename)
            metadata.Upload()
            log.Upload()
            os.remove(metadata_filename)
            os.remove(processed_frame_filename)
        except Exception as e:
            return jsonify({"error": "Nepodarilo sa autentifikovať pre gDrive.", "status": 206}), 206
        return jsonify({"ok": "Spracovaný log s metadátami úspešne nahraté.", "status": 200}), 200


def set_metadata(metadata, id, time, user, filename):
    """
    Pridanie informácii o uživateľovi, čase nahrania, identifácii spracovaného
    logovacieho súboru do metadát.
    """
    metadata_time = datetime.strptime(time, '%Y-%m-%dT%H:%M:%S.%fZ')
    metadata["id"] = id
    metadata["time"] = datetime.strftime(metadata_time, '%d %b %Y %H:%M:%S')
    metadata["user"] = user
    metadata["filename"] = filename[:-5]
    return metadata


@save.route("/load_processed_logs", methods=["GET", "POST"])
def load_processed_logs():
    """
    Získanie zoznamu dostupných a spracovaných logov.
    """
    drive = g_Drive()
    folder_token = drive.get_directory_token()
    drive_connection = drive.get_drive()
    file_list = drive_connection.ListFile({"q": f"'{folder_token}' in parents and trashed=false and mimeType='application/json'"}).GetList()
    metadata = list()
    for file in file_list:
        f = drive_connection.CreateFile({"id": file["id"]})
        metadata.append(
            set_metadata(
                json.loads(f.GetContentString(remove_bom=True))["metadata"],
                file["id"],
                file["createdDate"],
                file["lastModifyingUser"]["emailAddress"],
                file["originalFilename"],
            )
        )
    return jsonify({"data": metadata})


@save.route("/load_stats", methods=["POST"])
def load_stats():
    """
    Načítanie metadát z Google Drivu, ktoré obsahujú zozbierané štatistiky a exploračnú analýzu dát.
    """
    if not request.method == "POST" or not request.args["id"]:
        return jsonify({"error": "Nepodarilo sa načítať štatistiky.", "status": 207}), 207
    try:
        drive_connection = g_Drive().get_drive()
        f = drive_connection.CreateFile({"id": request.args["id"]})
        stats = json.loads(f.GetContentString(remove_bom=True))
    except:
        return jsonify({"error": "Nepodarilo sa získať štatistiky z google drivu.", "status": 208}), 208
    return jsonify(stats), 200


@save.route('/delete_processed', methods=['POST'])
def delete_processed_frame_and_metadata():
    """
    Zmazanie spracovaneho datoveho ramca a statistik z google drive.
    """
    deleted = g_Drive().delete_files([f"{request.form.get('frame_id')}.pkl",f"{request.form.get('frame_id')}.json"])
    if deleted:
        return jsonify({'ok' : 'Dátový rámec spoločne s metadátami zmazané.'})
    else:
        return jsonify({'error' : 'Nepodarilo sa zmazať dátový rámec s metadátami.'})


@save.route('/change_folder_token', methods=['GET', 'POST'])
def set_new_folder_token():
    """
    Nastavenie noveho priecinku pre ukladanie spracovanych datovych ramcov na google drive.
    """
    token = request.form.get('new_token')
    if not token or token == request.form.get('current_token'):
        return jsonify({"error": "Nie je možné nastaviť tento token.", "status": 201}), 201
    with open('folder_token.txt', 'w') as f_token:
        f_token.write(token)
    return jsonify({"ok": "Token úspešne zmenený.", "token": token }), 200


@save.route('/check_folder_data', methods=['POST'])
def check_folder_data():
    """
    Ziskanie aktualneho obsahu vyuzivaneho priecinku na google drive.
    """
    return jsonify({'data' : g_Drive().find_files()})


@save.route('/find_available_folders', methods=['POST'])
def find_folders():
    """
    Ziskanie dostupnych priecinkov na google drive.
    """
    return jsonify({'data' : g_Drive().find_folders()})


@save.route('/create_folder', methods=['POST'])
def create_folder():
    """
    Vytvorenie priecinku na google drive.
    """
    try:
        foldername = request.form.get('foldername')
        created = g_Drive().create_folder(foldername)
        if not created:
            return jsonify({"error": "Nepodarilo sa vytvoriť priečinok.", "status": 201}), 201
        return jsonify({"ok": "Priečinok bol úspešne vytvorený.", "status": 200}), 200
    except:
        return jsonify({"error": "Nemožno komunikovať s Google Drive API. Skúste sa autentifikovať.", "status": 201}), 201


@save.route('/relog', methods=['POST'])
def relog():
    """
    Zmazanie tokenu a identifikatoru aktualne vyuzivaneho priecinku. Nasledne vyziadanie dalsieho prihlasenia.
    """
    tokens = ['folder_token.txt', 'token.json']
    for token in tokens:
        if os.path.isfile(token):
            try:
                os.remove(token)
            except:
                return jsonify({"error": "Nepodarilo sa odhlásiť zo služby google drive."}), 201
    """
    vyziadanie prihlasenia
    """
    try:
        _ = g_Drive()
    except:
        return jsonify({"error": "Nepodarilo sa vyžiadať prihlásenie do služby google drive."}), 201
    return jsonify({"ok": "Prihlásenie úspešné."}), 200
