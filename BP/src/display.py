"""
Autor: Samuel Valaštín
Dátum : 28.1.2022

    Súbor funkcii pre zobrazenie HTML šablón.

"""
from flask import Blueprint, render_template, request, jsonify, send_file
from .save import g_Drive
import pandas as pd
import plotly
import plotly.express as px
import plotly.graph_objects as go
import json
import os


display = Blueprint('display', __name__)


class FrameGui:
    columns = {
        'SessionID' : 'Sedenie',
        'UserID' : 'Uživateľ',
        'Path' : 'Cesta',
        'PathLen' : 'Počet navštívených stránok',
        'SessionDuration' : 'Dĺžka uživateľského sedenia (s)',
        'Average time pp' : 'Čas strávený na stránku (s)',
    }


@display.route('/', methods=['GET'])
def home_template_view():
    """
    Render sablony pre domovsku stranku, ktora obsahuje rozcestnik a datasety.
    """
    return render_template('/home.html')


@display.route('/preprocess', methods=['GET'])
def preprocess_view():
    """
    Render sablony pre predspracovanie webovych pristupovych logov.
    """
    return render_template('/preprocessing.html')


@display.route('/processed_logs', methods=['GET'])
def processed_logs_view():
    """
    Render sablony pre zobrazenie zoznamu predspracovanych logov
    """
    return render_template('/processed_logs.html')


@display.route('/about', methods=['GET'])
def about_view():
    """
    Renderovanie sablony pre zobrazenie informacii o projekte.
    """
    return render_template('/about.html')


@display.route('/setup_mining', methods=['GET', 'POST'])
def setup_mining():
    """
    Render sablony pre nastavenie vstupnych parametrov pre dolovanie znalosti.
    """
    return render_template('setup_mining.html')

@display.route('/download_dataset/<id>', methods=['GET'])
def download_dataset(id):
    """
    Stiahnutie dostupneho datoveho ramca.
    """
    dataset_location = os.path.join(os.getcwd(), 'datasety')
    available_datasets = {
        '1' : f'{dataset_location}/UNI-IS-August.log.gz',
        '2' : f'{dataset_location}/UNI-IS-Sep-330K.log.gz',
        '3' : f'{dataset_location}/UNI-IS-Sep-250K.log.gz',
        '4' : f'{dataset_location}/UNI-IS-join.zip',
        '5' : f'{dataset_location}/usask-common-sample.log.gz',
        '6' : f'{dataset_location}/usask-common-jun.log.gz',
        '7' : f'{dataset_location}/usask-common-jul.log.gz',
        '8' : f'{dataset_location}/PHP-fusion.log.gz',
    }
    return send_file(available_datasets[id], as_attachment=True)


@display.route('/download_pdf', methods=['GET', 'POST'])
def download_thesis():
    """
    Stiahnutie pdf po vyziadani.
    """
    return send_file(os.path.join(os.getcwd(), 'projekt.pdf'), as_attachment=True)        


@display.route('/change_folder_token', methods=['GET', 'POST'])
def load_folder_token():
    """
    Nacitanie aktivneho priecinku urceneho pre ukladanie suborov na google drive.
    """
    exists = os.path.isfile('folder_token.txt')
    if not exists:
        current_token = 'Aktuálne nie je zvolený žiaden token'
    else:
        with open('folder_token.txt', 'r') as f_token:
            current_token = f_token.read().rstrip()
    if 'HEROKU' in os.environ:
        return render_template('change_folder_token.html', current_token=current_token, localhost='False')
    else:
        return render_template('change_folder_token.html', current_token=current_token, localhost='True')


def fix_displaying_paths(paths):
    """
    Uprava zobrazenia uzivatelskych ciest pri zobrazeni datasetov.
    """
    for i, path in enumerate(paths):
        paths[i] = [f'<br>{page}' if (index % 4) == 0 and index > 3 else page for index, page in enumerate(path)]
        paths[i] = ', '.join(paths[i])
    return paths


@display.route('/load_dataset', methods=['GET'])
def load_and_display_processed_dataset():
    """
    Nacitanie spracovaneho datasetu.
    """
    df = g_Drive().get_dataframe(request.args.get('file') + '.pkl')
    if df.empty:
        return jsonify({'error' : 'Nemožno načítať spracovaný dataset.'})
    else:
        df = df.rename(columns=FrameGui.columns)
        df['Čas strávený na stránku (s)'] = df['Čas strávený na stránku (s)'].round(decimals=1)
        values = df.transpose().values.tolist()
        values[2] = fix_displaying_paths(values[2])
        fig = go.Figure(data=[go.Table(
            columnwidth = [65, 65, 900, 185, 205, 195],
            header=dict(values=[f"<b>{column}</b>" for column in df.columns], height=25),
            cells=dict(values=values, height=40, align='left')),            
        ])
        fig.update_layout(template='plotly_white', margin=dict(r=0, l=0,t=100,b=0), height=800)
        return render_template('/display_dataset.html', table=json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder))