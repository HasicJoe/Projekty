"""
Autor: Samuel Valaštín
Dátum : 28.1.2022
    Trieda urcena pre predspracovanie webovych pristupovych logov.
    Predspracovanie zahrna:
        = spracovanie rozsirenych uzivatelskych moznosti
        = vyuzitie niektorych predspracovanych dat z štatistického modulu
        = subezne zbieranie informacii pocas cistenia logov
        = odfiltrovanie logovacich zaznamov vytvorenych botmi
        = zbavenie sa HTTP metod, ktore niesu vhodne pre analyzu webovych pristupov
        = zbavenie sa neuspesnych HTTP ziadosti
        = konverziu datumu a casu na casove razitko
        = detekciu uzivatelov
        = rozdelenie uzivatelskych zaznamov do uzivatelskych sedeni
        = vytvorenie ciest uzivatelskych sedeni
        = exploracnu analyzu dat
        = docasne ulozenie spracovaneho datoveho ramca
"""


from .detect import FormatAnalyzer, SupportedHeaders
from .stats import Stats
import re, os, time, datetime, json, uuid
from flask import Blueprint, request, jsonify
import pandas as pd
import swifter
import numpy as np
from crawlerdetect import CrawlerDetect
from urllib.parse import urlparse
import mimetypes
import plotly
import plotly.express as px
from user_agents import parse # pip install pyyaml ua-parser user-agents
import plotly.graph_objects as go


analyze = Blueprint('analyze', __name__)


class Cleaner:
    def __init__(self, options, df, log_format, stats_df = None, accepted_extensions = None):
        """
        Proces predspracovania webovych pristupovych logov
        Podporovane logovacie formaty -> COMMON a COMBINED
        """
        self.set_options(options)
        if log_format == SupportedHeaders.COMBINED:
            self.df = df[['Remote Host', 'Date and Time', 'Request', 'Referer', 'User Agent', 'State Code']]
        elif log_format == SupportedHeaders.COMMON:
            self.df = df[['Remote Host', 'Date and Time', 'Request', 'State Code']]
        if stats_df is None:
            self.detect_missing_data(log_format)
        else:
            self.df = pd.concat([self.df, stats_df], axis=1)
        self.counter = {
            "Formát" : "Combined" if log_format == SupportedHeaders.COMBINED else "Common",
            "Celkový počet načítaných záznamov" : self.df.shape[0],
        }
        self.counter_header = f"Report -> {df['Date and Time'].min()} - {df['Date and Time'].max()}"
        self.df = self.df.drop(['Request'], axis=1)
        self.filter_automatic_requests()
        self.counter['Počet záznamov po zbavení sa nepotrebných súborov'] = self.df.shape[0]
        self.filter_unnec_methods()
        self.filter_invalid_sc()
        self.counter['Počet záznamov s úspešným navratovým kódom'] = self.df.shape[0]
        self.remove_crawlers()
        self.counter['Počet záznamov po zbavení sa botov'] = self.df.shape[0]
        self.datetime_to_timestamp()
        self.clean_urls(log_format)
        self.user_identification(log_format)
        self.counter['Počet unikátnych uživateľov'] = self.df['UserID'].nunique()
        if log_format == SupportedHeaders.COMBINED:
            self.df = self.df.drop(['Remote Host','User Agent'], axis=1)
        else:
            self.df = self.df.drop(['Remote Host'], axis=1)
        self.sessionization(log_format)
        self.counter["Počet uživateľských sedení"] = self.df['SessionID'].nunique()
        self.make_path_df(self.complete_path(log_format, Paths()))
        self.counter['Počet uživateľských sedení (>1)'] = len(self.path_df[self.path_df['PathLen'] > 1])
      

    def set_options(self, options):
        """
        Spracovanie rozšírených uživateľských možností získaných z GUI.
        """
        self.session_trashold = int(options['session_trashold']) * 60
        self.accepted_extensions = re.split(',', re.sub(' +', '',options['extensions']))

    def filter_invalid_sc(self):
        """
        Zbavenie sa zaznamov s navratovym kodom <0,199> <300, X>
        """
        self.df = self.df.loc[(self.df['State Code'] >= 200) & (self.df['State Code'] < 300)]
        self.df = self.df.drop(['State Code'], axis=1)
      

    def datetime_to_timestamp(self):
        """
        Transformacia na casoveho razitka ziadosti. 
        """
        self.df['Timestamp'] = self.df['Date and Time'].values.astype(np.int64) // 10 ** 9
        self.df = self.df = self.df.drop(['Date and Time'], axis=1)
        """
        ziskanie rozdielu od prvej ziadosti daneho sedenia
        """
        min_request_time = self.df['Timestamp'].min()
        self.df['Timestamp'] = self.df['Timestamp'] - min_request_time


    def filter_automatic_requests(self):
        """
        Zbavenie sa nepotrebnych 'requestov' pocnuc grafickymi/zvukovymi subormi.
        """
        self.df = self.df[self.df['Extension'].isin(self.accepted_extensions)]
        self.df = self.df.drop(['Extension'], axis=1)


    def remove_crawlers(self):
        """
        Zbavenie sa zaznamov, kt. boli vykonane botmi/crawlermi.
        """
        self.df = self.df[self.df['crawlers'] == False]
        self.df = self.df.drop(['crawlers'], axis=1)


    def filter_unnec_methods(self):
        """
        Odfiltrovanie nepotrebnych HTTP metod okrem GET a POST.
        """
        self.df = self.df[self.df['Method'].isin(['GET', 'POST'])]

    
    def detect_missing_data(self, log_format):
        """
        Spracovanie potrebných dát pred samotným očistením dát.
        """
        self.df['Method'] = self.df['Request'].str.split(' ').str[0]
        self.df['URL'] = self.df['Request'].str.split(' ').str[1] #.swifter.apply(lambda x: urlparse(x).path)
        self.df = self.df[(~self.df['URL'].isnull())]
        self.df['Extension'] = self.df['URL'].swifter.apply(lambda x: os.path.splitext(x)[1])
        self.crawler_parser = CrawlerDetect()
        
        """
        detekcia botov/crawlerov pre logovacie zaznamy COMMON nemozno odfiltrovat ziadosti vykonane botmi 
        """
        if log_format == SupportedHeaders.COMBINED:
            agents = pd.Series(self.df['User Agent'].unique())
            crawlers = agents.swifter.apply(lambda x: self.crawler_parser.isCrawler(x))
            self.df_crawlers = pd.DataFrame(data=dict(Agent=agents,Crawler=crawlers))
            self.crawler_list = self.df_crawlers.loc[self.df_crawlers['Crawler'] == True, ['Agent']]
            self.df['crawlers'] = self.df['User Agent'].isin(self.crawler_list['Agent'])
        elif log_format == SupportedHeaders.COMMON:
            self.df.loc[:, "crawlers"] = False
    

    def clean_urls(self, log_format):
        """
        Vycistenie URL a transformacia formatu z -> https://page.com/index.php?id=64646484786 -> /index.php
        """
        if log_format == SupportedHeaders.COMBINED or log_format == SupportedHeaders.COMMON:
            self.df['URL'] = self.df['URL'].swifter.apply(lambda x: urlparse(x).path)
            if log_format == SupportedHeaders.COMBINED:
                self.df['Referer'] = self.df['Referer'].swifter.apply(lambda x: urlparse(x).path)

    
    def user_identification(self, log_format):
        """
        Identifikacia uzivatelov na zaklade spojenia poli IP (IP + User Agent pre Combined)
        """
        if log_format == SupportedHeaders.COMMON:
            self.df['UserID'] = self.df.groupby(['Remote Host']).ngroup()
        elif log_format == SupportedHeaders.COMBINED:
            self.df['UserID'] = self.df.groupby(['Remote Host', 'User Agent']).ngroup()
    

    def sessionization(self, log_format):
        """
        Rozdelenie uzivatelov na uzivatelske sessions.
        """
        self.df = self.df.sort_values(['UserID', 'Timestamp'])
        if log_format == SupportedHeaders.COMMON or log_format == SupportedHeaders.COMBINED:
            """
            rozdelenie za pomoci kumulovanej sumy v pripade ak v zoradenom zozname zaznamov presiahne rozdiel medzi dvoma
            riadkami prah alebo ak sa lisi identifikator uzivatela -> inkrementacia
            """
            self.df['SessionID'] = ((self.df['Timestamp'] - self.df['Timestamp'].shift(1) > self.session_trashold) | (self.df['UserID'] != self.df['UserID'].shift(1))).cumsum()


    def back_track(self, path, referer, url):
        """
        Funkcia vyhlada index polozky, ktora sa zhoduje s polom referer sucasneho requestu.
        Nasledne pre potreby 'kompletizacie cesty'
        """
        for index, item in reversed(list(enumerate(path))):
            if item == referer:
                back_track = list(reversed(path[index:-1]))
                if back_track is not None:
                    return path + back_track + [url]
        path.append(url)
        return path
    

    def complete_path(self, log_format, paths):
        """
        Kompletizacia uzivetelskych ciest pre jednotlive sessions a nasledne ulozenie jednotlivych sessions.
        """
        grouped_sessions = self.df.groupby(['SessionID'])
        sessions = list(self.df['SessionID'].unique())
        if log_format == SupportedHeaders.COMBINED:
            for session in sessions:
                """
                nacitanie ramca obsahujuceho dane uzivatelske sedenie
                """
                session_df = grouped_sessions.get_group(session).copy().reset_index()
                session_dur = session_df['Timestamp'].max() - session_df['Timestamp'].min()
                """
                vytvorenie poradia uzivatelskych poziadavok 1, 2, ... , n
                """
                session_df['Hit'] = np.arange(1, int(session_df.shape[0] + 1))
                session_path = list()
                session_last_url, user_id = None, None
                for item in session_df.itertuples(index=False):
                    if item.Hit == 1:
                        """
                        prva poziadavka -> prazdny odkazovatel
                        """
                        session_path.append(item.URL)
                        session_last_url = item.URL
                        user_id = item.UserID
                    else:
                        """
                        prazdne pole Odkazovatela -> nemozno detekovat chybajuce cesty
                        """
                        if len(item.Referer) < 2:
                            session_path.append(item.URL)
                            session_last_url = item.URL
                        elif item.Method == 'GET':
                            """
                            v pripade ak sa lisi posledna vyziadana URL adresa s polom odkazovatela vykona sa
                            kontrola chybajucich stranok na ceste
                            """
                            if session_last_url == item.Referer:
                                session_path.append(item.URL)
                            else:
                                session_path = self.back_track(session_path, item.Referer, item.URL)
                            session_last_url = item.URL
                        elif item.Method == 'POST':
                            """
                            pri HTTP metode post nemozno vykonat kontrolu chybajucej cesty
                            """
                            if not item.Referer == session_last_url:
                                session_path.append(item.Referer)
                            else:
                                session_path.append(item.URL)
                paths.add_path(session, user_id, session_path, session_dur)
        elif log_format == SupportedHeaders.COMMON:
            """
            chyba pole odkazovatel, nemozno vyhladat chybajuce cesty
            """
            for session in sessions:
                session_df = grouped_sessions.get_group(session).copy().reset_index()
                session_dur = session_df['Timestamp'].max() - session_df['Timestamp'].min()
                session_path = session_df['URL'].tolist()
                user_id = int(session_df['UserID'].unique())
                paths.add_path(session, user_id, session_path, session_dur)
        return paths
    

    def make_path_df(self, paths):
        """
        Vytvorenie rámca ciest zo slovníku po spracovaní ciest.
        """
        self.path_df = pd.DataFrame(data=paths.path_dict)


class Paths:
    def __init__(self):
        """
        Inicializacia slovniku do ktoreho budu ulozene cesty uzivatelskych sessions.
        """
        self.path_dict = {
            "SessionID" : [],
            "UserID" : [],
            'Path' : [],
            'PathLen' : [],
            "SessionDuration" : []
        }


    def add_path(self, session_id, user_id, path, duration):
        """
        Pridanie záznamu o ceste session. V pripade ak je v
        ceste uzivatelskeho sedenia ziadost o subor '/robots.txt' ->
        ide o podozrive spravanie a tak je dane uzivatelske sedenie 
        neulozene.
        """
        if not '/robots.txt' in path:
            self.path_dict['SessionID'].append(session_id)
            self.path_dict['UserID'].append(user_id)
            self.path_dict['Path'].append(path)
            self.path_dict['PathLen'].append(len(path))
            self.path_dict['SessionDuration'].append(duration)


def make_cleaning_table(data, table_header):
    """
    Vytvorenie tabulky pre zobrazenie statistik ziskanych pri predspracovani.
    """
    keys, values = list(), list()
    for k, v in data.items():
        keys.append(f"<b>{k}</b>")
        values.append(v)
    fig =  go.Figure(
        data=[go.Table(
            columnwidth = [450, 100],
            cells=dict(
                values=[
                    keys,
                    values,
                ],
                align='left',
                height=30,
                font_size=13,
            ),
        )]
    )
    """
    zbavenie sa zahlavia tabulky a uprava vzhladu tabulky
    """
    fig.for_each_trace(lambda x: x.update(header_fill_color = 'rgba(0,0,0,0)'))
    fig.update_layout(template='seaborn', title_text=table_header, title_x=0, title_y=0.96, height=350)
    fig.update_layout(margin=dict(r=0, l=0,t=30,b=0))
    return fig


def make_visit_actions(session_df):
    """
    Vytvorenie rámca pre graf počtu uživateľských návštev.
    """
    session_df.loc[:, 'Count'] = 1
    session_df =  session_df.groupby(['PathLen']).sum().reset_index()
    return session_df.loc[session_df['Count'] > 0.0]


def make_session_duration(session_df):
    """
    Vytvorenie rámca pre graf dĺžky uživateľskych sessions.
    """
    session_df = session_df[session_df['PathLen'] > 1]
    session_df = session_df.drop(['PathLen'], axis=1)
    session_df['SessionDuration'] = session_df['SessionDuration'].round(decimals=-1)
    session_df.loc[:, 'Count'] = 1
    return session_df.groupby(['SessionDuration']).sum().reset_index()
    

def make_average_time_per_page(session_df):
    """
    Vytvorenie rámca pre graf priemernej dĺžky strávenej na stránke pre jednotlivé sessions.
    """
    session_df = session_df[session_df['PathLen'] > 1]
    session_df['Average time pp'] = session_df['SessionDuration'] / (session_df['PathLen'] - 1)
    session_df = session_df.drop(['PathLen', 'SessionDuration'], axis=1)
    session_df.loc[:, 'Count'] = 1
    session_df['Average time pp'] = session_df['Average time pp'].round(decimals=0)
    session_df =  session_df.groupby(['Average time pp']).sum().reset_index()
    return session_df.loc[session_df['Count'] > 0.0]


def add_session_button(name, visib_index, layout_range = 3):
    """
    Pridanie tlačidla pre prepínanie kontextu pre prepínanie grafov.
    """
    return dict(
        method="restyle",
            label=name,
            args=[
                {
                    'visible' : [True if visib_index == n else False for n in range(layout_range)],
                }
            ],
        )


def add_session_trace(df, x , y, is_visible=False, N = 50, horizontal=False):
    """
    Pridanie stopy grafu pre viacstopový graf s možnosťou prepínania os.
    Pre graficke účely je vybraných N najčastejších hodnôt s časovým podvzorkovaním.
    """
    df = df.nlargest(N, y)
    df = df.sort_values(by=[x])
    return go.Bar(
        x=df[x].to_list(),
        y=df[y].to_list(),
        text=df[y].to_list(),
        textposition='auto',
        visible=is_visible,
        orientation = 'h' if horizontal else 'v',
    )
    
    
def make_sessions_stats(session_df):
    """
    Riadenie exploračnej analýzy a štatistiky exploračnej analyzy dát.
    """
    visit_actions_df = make_visit_actions(session_df[['PathLen']].copy())
    session_duration_df = make_session_duration(session_df[['SessionDuration', 'PathLen']].copy())
    avg_time_pp_df = make_average_time_per_page(session_df[['SessionDuration', 'PathLen']].copy())
    fig, button = go.Figure(), list()
    fig.add_trace(add_session_trace(session_duration_df, 'SessionDuration', 'Count', is_visible=True))
    fig.add_trace(add_session_trace(visit_actions_df, 'PathLen', 'Count'))
    fig.add_trace(add_session_trace(avg_time_pp_df, 'Average time pp', 'Count'))
    button.append(add_session_button("Časové dĺžky uživateľských sedení (podvzorkované 10s)", 0))
    button.append(add_session_button('Počet navštívených stránok', 1))
    button.append(add_session_button("Priemerný čas na stránku (podvzorkované 1s)", 2))
    fig.update_layout(
        template="plotly",
        height=450,
        width=820,
        showlegend=False,
        title="Informácie o sedeniach",
        title_x=0.07,
        title_y=0.92,
        updatemenus=[
            {
                "buttons": button,
                "direction": "down",
                "showactive": True,
                "x": 0.75,
                "y": 1.15,
            }
        ],
        margin=dict(r=10, l=10,t=60,b=60),
    )
    fig.update_xaxes(type='category', title='Dĺžka')
    fig.update_yaxes(title='Počet')
    return fig


def dump_stats_data(stats_figures):
    """
    Uloženie dát zozbieraných zo štatistík pre grafické uživateľské prostredie.
    """
    objs = ['table', 'crawlers', 'os', 'utili']
    json_data = dict()
    for obj in objs:
        json_data[obj] = json.dumps(stats_figures[obj], cls=plotly.utils.PlotlyJSONEncoder)
    json_data['status'] = 200
    return json_data


def session_relation(session_df):
    """
    Vytvorenie grafu obsahujúceho Lineárnu regresiu medzi počtom navštívených stránok
    a dĺžkou sessions.
    """
    return px.scatter(session_df, x='Počet navštívených stránok', y='Dĺžka sedenia(sec)', 
      trendline='ols', trendline_color_override='orangered', width=780, height=450, 
      opacity=0.6, title="Lineárna regresia"
    )


def make_regression(session_df):
    """
    Spracovanie rámca a aktualizácia rozloženia pre graf Lineárnej regresie.
    """
    session_rel_df = session_df[['SessionDuration', 'PathLen']].copy()
    session_rel_df = session_rel_df.rename(columns={"SessionDuration" : "Dĺžka sedenia(sec)", "PathLen" : "Počet navštívených stránok"})
    fig = session_relation(session_rel_df)
    fig.update_layout(margin=dict(r=0, l=0,t=60,b=60), title_x=0.55, title_y=0.92)
    return fig


def add_request_trace(df, y, x, visible=False):
    """
    Vytvorenie stopy pre vizualizaciu TOP 10 prvych posledných navštívených stránok v rámci už. sedení.
    """
    return go.Bar(
        x=df[x].to_list(),
        y=df[y].to_list(),
        text=df[x].to_list(),
        textposition='auto',
        visible=visible,
        orientation = 'h',
        hovertemplate = 'Počet: <b>%{text}</b><br>Stránka: <b>%{y}</b><extra></extra>', # prepisanie chyby kniznice
    )


def first_request_bar(first_request_df, last_request_df):
    """
    Vizualizacia TOP 10 prvých/poslednych prístupov na stránku v rámci uživateľských sedení.
    """
    fig, button = go.Figure(), list()
    fig.add_trace(add_request_trace(first_request_df, 'Prvá požiadavka', 'Počet', visible=True))
    fig.add_trace(add_request_trace(last_request_df, 'Posledná požiadavka', 'Počet'))
    button.append(add_session_button('Prvá navštívená stránka', 0, layout_range=2))
    button.append(add_session_button('Posledná navštívená stránka', 1, layout_range=2))
    fig.update_layout(
            template='plotly_white',
            height=500,
            width=950,
            showlegend=False,
            title='Top 10 - Prvá/posledna navštívená stránka sedenia',
            margin_pad=10,
            yaxis={'title' : 'Stránka'},
            xaxis={'title': 'Počet'},
            title_x=0.05,
            title_y=0.97,
            updatemenus=[
                {
                    "buttons": button,
                    "direction": "down",
                    "showactive": True,
                    "x": 0.65,
                    "y": 1.15,
                }
            ],
            margin=dict(r=0, l=0,t=50,b=0),
        )
    return fig


def first_request_page_stats(session_df):
    """
    Spracovanie rámca uživateľských sedení pre identifikáciu TOP 10 prvých prístupov na stránku v rámci uživateľských sedení.
    """
    first_request_df = session_df[['Path']].copy()
    last_request_df = session_df[['Path']].copy()
    """
    ziskanie prvych a poslednych poziadaviek pre jednotlive uzivatelske sedenia
    """
    first_request_df['Prvá požiadavka'] = first_request_df['Path'].str[0]
    last_request_df['Posledná požiadavka'] = last_request_df['Path'].str[-1]
    first_request_df.loc[:, 'Počet'] = 1
    last_request_df.loc[:, 'Počet'] = 1
    first_request_df = first_request_df.groupby(['Prvá požiadavka']).sum().reset_index().nlargest(10, 'Počet')
    last_request_df = last_request_df.groupby(['Posledná požiadavka']).sum().reset_index().nlargest(10, 'Počet')
    return first_request_bar(first_request_df, last_request_df)


def convert_metadata_to_gui(metadata):
    """
    Konverzia kĺǔčov metadát pre jednoduchšie spracovanie pre GUI.
    """
    old_keys = [
        'Počet záznamov po zbavení sa botov',
        'Formát',
        'Počet unikátnych uživateľov', 
        'Počet uživateľských sedení',
        'Počet uživateľských sedení (>1)'
    ]
    new_keys = ['n_of_items', 'format', 'n_of_users', 'n_of_sessions', 'n_of_multiple_sessions']
    for n_k, o_k in zip(new_keys, old_keys):
        metadata[n_k] = metadata.pop(o_k)
    return metadata


def add_cleaner_data(figures, counter, path_frame, table_header, set_status = False):
    """
    Uloženie dát získaných z exploračnej analýzy do slovníku a rovnako uloženie úspešného návratového kódu
    pre komunikáciu s GUI.
    """
    if set_status:
        figures['status'] = 200
    figures['cleaner'] = json.dumps(make_cleaning_table(counter, table_header), cls=plotly.utils.PlotlyJSONEncoder)
    figures['regression'] = json.dumps(make_regression(path_frame), cls=plotly.utils.PlotlyJSONEncoder)
    figures['session_stats'] = json.dumps(make_sessions_stats(path_frame), cls=plotly.utils.PlotlyJSONEncoder)
    figures['first_request'] = json.dumps(first_request_page_stats(path_frame), cls=plotly.utils.PlotlyJSONEncoder)
    figures['metadata'] = convert_metadata_to_gui(counter)
    return figures


def make_and_return_directory():
    """
    Prípadné vytvorenie adresára pre dočasné uloženie dát.
    """
    path = os.path.join(f"{os.getcwd()}/tmp")
    if not os.path.isdir(path):
        os.makedirs(path)
    return path


def clean_unsaved_data(curr_time):
    """
    Vymazanie všetkých docasne ulozenych súborov, po uplynutí 15 minút od uloženia.
    """
    tmp_dir = f"{os.getcwd()}/tmp"
    tmp_files = [os.path.join(tmp_dir, file) for file in os.listdir(tmp_dir) if os.path.isfile(os.path.join(tmp_dir, file))]
    tmp_data_and_time = [(file, curr_time - os.path.getctime(file))for file in tmp_files]
    for file, time in tmp_data_and_time:
        if time > 60 * 15:
            os.remove(file)


def add_average_time(processed_df):
    """
    Ulozenie priemerneho casu straveneho na stranku pocas uzivatelskeho sedenia.
    """
    processed_df = processed_df[processed_df['PathLen'] > 1]
    processed_df['Average time pp'] = processed_df['SessionDuration'] / (processed_df['PathLen'] - 1)
    return processed_df


def is_active_folder_available():
    """
    Kontrola, ci je zvoleny aktivny priecinok pre ulozenie na google drive.
    """
    try:
        if os.path.isfile('folder_token.txt') and os.path.getsize('folder_token.txt') > 0:
            return True
        else:
            return False
    except:
        return False


def temporary_save_analyze_data(figures, processed_df):
    """
    Dočasné uloženie dát, po potvrdení uživateľom sú dáta
    presmerované na Google Drive.
    """
    file_path = make_and_return_directory() + "/" + str(uuid.uuid4()) + '.pkl'
    clean_unsaved_data(time.time())
    processed_df = add_average_time(processed_df)
    processed_df.to_pickle(file_path)
    metadata_path = file_path[:-4] + '.json'
    with open(metadata_path, 'w') as metadata_f:
        json.dump(figures, metadata_f)
    figures['metadata'] = metadata_path
    if not is_active_folder_available():
        figures['unable'] = 'Chýba aktívny priečinok. Nie je možné uložiť dáta.'
    return figures


@analyze.route("/file_upload", methods=["GET", "POST"])
def file_upload():
    """
    Spravovanie logovacieho súboru a rozšírených možností,
    detekcia dát, prípadné získanie štatistík z logovacieho
    súboru a následné predspracovanie dát.
    """
    options = {
        'extensions' : request.form.get('extensions'),
        'session_trashold' : request.form.get('session_tr'),
        'stats_on' : request.form.get('stats'),
        'log_file' : request.files.get('log-file[0]') 
    }
    if request.method == "POST" and options['log_file']:
        start = time.time()
        analyzer = FormatAnalyzer(options['log_file'])
        if analyzer.error:
            return jsonify({'error' : 'Neznámy súbor / nemožno načítať dátovú sadu', 'status' : 202}), 202
        if analyzer.df is not None:
            if analyzer.format == SupportedHeaders.COMBINED or analyzer.format == SupportedHeaders.COMMON:
                if options['stats_on'] == 'true':
                    stats = Stats(analyzer.df, analyzer.format)
                    response_data = dump_stats_data(stats.figures)
                    cleaner = Cleaner(options, analyzer.df, analyzer.format, stats.df_cleaner)
                else:
                    response_data = dict()
                    cleaner = Cleaner(options, analyzer.df, analyzer.format)
                response_data = add_cleaner_data(response_data, cleaner.counter, cleaner.path_df, cleaner.counter_header, set_status=True)
                response_data = temporary_save_analyze_data(response_data, cleaner.path_df)
                return jsonify(response_data), 200
            else:
                """
                ponechane pre buduce rozsirenie aplikacie
                """
                pass
        return jsonify({'error' : 'Neznámy súbor / nemožno načítať dátovú sadu', 'status' : 202}), 202
    return jsonify({'error' : 'Neplatná požiadavka', 'status' : 201}), 201