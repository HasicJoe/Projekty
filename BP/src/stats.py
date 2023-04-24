"""
Autor: Samuel Valaštín
Dátum : 28.1.2022
Súbor funkcii pre získavanie agregoraných štatistík počas predspracovania webových prístupových logov.
    Okrem získavania štatistík vykonáva tento 'štatisticky modul' v pripade vyuzitia, ktore je volitelne
    aj detekciu webových botov/crawlerov a upravu niektorych poli, ktore mozno nasledne nie je nutne vo 
    faze cisteho predspracovania logov vyuzit.

"""


from flask import Blueprint
from .detect import SupportedHeaders
import os
import mimetypes
import pandas as pd
import numpy as np
import swifter
from crawlerdetect import CrawlerDetect
from urllib.parse import urlparse
import plotly
import plotly.express as px
from user_agents import parse
import plotly.graph_objects as go

stats = Blueprint("stats", __name__)


class Stats:
    def __init__(self, df, format):
        """
        Trieda pre zozbieranie volitelnych statistik pocas predspracovania
        """
        df = df.drop(['Remote Host'], axis=1)
        mimetypes.add_type("application/x-httpd-php", ".php")
        self.crawler_parser = CrawlerDetect()
        self.figures = {"table" : None, "crawlers" : None, "os" : None, "utili" : None}
        self.detect_crawlers(df, format)
        self.detect_mime_types(df)
        df = df.drop(['Request'], axis=1)
        self.df_cleaner = self.make_df_for_cleaner(df)
        self.make_utilization_stats(df)
        df = df.drop(['Date and Time'], axis=1)
        self.make_mime_stats(df)
        df = df.drop(['Extension', 'Mime type'], axis=1)
        self.make_spiders_stats(df)
        df = df.drop(['State Code'], axis=1)
        if format == SupportedHeaders.COMBINED:
            self.make_user_agent_stats(df)
        elif format == SupportedHeaders.COMMON:
            self.make_url_stats(df)

    
    def detect_crawlers(self, df, format):
        """ 
        Detekcia crawlerov/botov - z dovodu vysokej casovej ceny .apply() nemozno pouzit
        """
        if format == SupportedHeaders.COMMON:
             df.loc[:, "crawlers"] = False
        elif format == SupportedHeaders.COMBINED:
            agents = pd.Series(df['User Agent'].unique())
            crawlers = agents.swifter.apply(lambda x: self.crawler_parser.isCrawler(x))
            self.df_crawlers = pd.DataFrame(data=dict(Agent=agents,Crawler=crawlers))
            self.crawler_list = self.df_crawlers.loc[self.df_crawlers['Crawler'] == True, ['Agent']]
            df['crawlers'] = df['User Agent'].isin(self.crawler_list['Agent'])
    

    def detect_mime_types(self, df):
        """
        Vycistenie URL od parametrov
        """
        df['Method'] = df['Request'].str.split(' ').str[0]
        df['URL'] = df['Request'].str.split(" ").str[1]
        df.dropna(subset=['URL'], inplace=True)
        df['Extension'] = df['URL'].swifter.apply(lambda x: os.path.splitext(x)[1] if str(x) else None)
        df['Mime type'] = df['URL'].swifter.apply(lambda x: mimetypes.guess_type(x)[0])
    

    def make_df_for_cleaner(self, df):
        """
        Výber stĺpcov potrebných pre predspracovanie dát pri nevyužití možnosti
        vytvorenia štatistík logov.
        """
        return df.loc[:, ['Method', 'URL', 'Extension', 'crawlers']]


    def make_utilization_stats(self, df):
        """
        Vytvorenie grafov časového využitia z logových záznamov s rôznymi časovými
        vzorkovaniami.
        """
        df.loc[:, "Count"] = 1
        df_time = df.loc[:, ['Count', 'Date and Time']]
        time_diff = (df_time['Date and Time'].max() - df_time['Date and Time'].min()).total_seconds()
        fig, buttons = go.Figure(), list()
        samples = [
            ('S', 'Vyťaženie [sec]'),
            ('T', 'Vyťaženie [min]'), 
            ('H', 'Vyťaženie [h]'), 
            ('D', 'Vyťaženie [d]'), 
            ('W', 'Vyťaženie [w]')
        ]
        # choose default layout and resample
        if time_diff > 60*60*24*7*6:
            sample =  ('W', 'Vyťaženie [w]')
        elif time_diff > 60*60*24*6:
            sample = ('D', 'Vyťaženie [d]')
        elif time_diff > 60*60*6:
            sample = ('H', 'Vyťaženie [h]')
        elif time_diff > 60*6:
            sample = ('T', 'Vyťaženie [min]')
        else:
            sample = ('S', 'Vyťaženie [sec]')
        df_base = df_time.set_index('Date and Time').resample(sample[0]).sum().reset_index()
        fig.add_trace(go.Bar(y=df_base['Count'].to_list(), x=df_base['Date and Time'].to_list(), text=df_base['Count'].to_list(), textposition='auto'))
        buttons.append(self.add_util_button(sample, df_time))
        for i, samp in enumerate(samples):
            if samp == sample:
                samples = samples[i+1::]
        for s in samples:
            buttons.append(self.add_util_button(s, df_time))
        fig.update_layout(
            template="plotly",
            height=600,
            title="Časové vyťaženie",
            title_x=0.1,
            title_y=0.88,
            xaxis={'title' : 'Čas'},
            yaxis={'title' : 'Počet žiadostí'},
            width=950,
            showlegend=False,
            updatemenus=[
                {
                    "buttons": buttons,
                    "direction": "down",
                    "showactive": True,
                    "x": 0.5,
                    "y": 1.1,
                }
            ],
        )
        self.figures['utili'] = fig


    def add_util_button(self, sample, df):
        """
        Vytvorenie tlačidla pre aktualizáciu hodnôt využitia s rôznym časovým prevzorkovaním.
        """
        df_time = df.set_index('Date and Time').resample(sample[0]).sum().reset_index()
        return dict(
            method="restyle",
            label=sample[1],
            args=[
                {
                    "y": [df_time["Count"].to_list()],
                    "x": [df_time["Date and Time"].to_list()],
                    "text": [df_time["Count"].to_list()],
                    "visible": True,
                    "xaxis_title" : "x axis title",
                    "yaxis_title" : sample[1],
                }
            ],
        )

 
    def mime_stats(self, df):
        """
        Spojenie a scitanie najcastejsich zaznamov podla mime typu a koncovky vyziadanych suborov.
        """
        df.loc[:, "Count"] = 1
        return (
            df.groupby(["Mime type", "Extension"])
            .sum()
            .reset_index()
            .sort_values(by="Count", ascending=False)
        )

    
    def make_mime_stats(self, df):
        """
        Vytvorenie prehladnej tabulky rozdelenu na zaklade rozdielnych mime typov a koncoviek suborov.
        Mimo ineho obsahuje aj informacie o detekovanych botoch v ziadostiach o subory roznych typov
        """
        mime = self.mime_stats(df)
        self.figures['table'] = go.Figure(
            data=[go.Table(
                columnwidth = [280, 140, 220, 170, 190],
                header=dict(
                    values=[
                        "<b>MIME typ</b>",
                        "<b>Koncovka</b>",
                        "<b>Objem prenosu (B)</b>",
                        "<b>Detekovaný crawler</b>",
                        "<b>Počet žiadostí</b>",
                    ]
                ),
                cells=dict(
                    values=[
                        mime["Mime type"],
                        mime["Extension"],
                        mime["Transfer Volume"],
                        mime["crawlers"],
                        mime["Count"],
                    ],
                    height=29,
                    font_size=13,
                ),
            )]
        )
        self.figures['table'].update_layout(template='plotly_white', margin=dict(r=0, l=0, t=60, b=10), height=380)
    

    def spider_stats(self, df):
        """
        Vypocet pocetnosti vyskytov kde bol detekovany bot/crawler vs nebol detekovany bot/crawler.
        """
        df.loc[:, "Count"] = 1
        return (
            df.groupby(["crawlers"])
            .sum()
            .reset_index()
            .replace({True: "Crawler detekovaný", False: "Žiadny crawler"})
        )


    def state_code_stats(self, df):
        """
        Vypocet pocetnosti vyskytov jednotlivych HTTP navratovych kodov.
        """
        df_sc = df[['State Code']].copy()
        df_sc.loc[:, "Count"] = 1
        return df_sc.groupby(['State Code']).sum().reset_index()


    def method_stats(self,df):
        """
        Vypocet pocetnosti vyskytov jednotlivych HTTP metod v logovacich zaznamoch.
        """
        df_methods = df[['Method']].copy()
        df_methods.loc[:, "Count"] = 1
        return df_methods.groupby(['Method']).sum().reset_index()


    def add_spider_button(self, df, y, x, label):
        """
        Vytvorenie tlacidla pre prepinanie zobrazovania grafov.
        """
        return dict(
            method='restyle',
            label=label,
            args=[
                {
                    'values' : [df[y].to_list()],
                    'labels' : [df[x].to_list()],
                    'visible' : True,
                    'textinfo' : 'label+percent',
                    'textposition' : 'inside',
                }
            ]
        )


    def make_spiders_stats(self, df):
        """
        Vytvorenie statistik pre detekciu botov, preneseny objem dat, najcastejsie HTTP navratove kody a najcastejsie metody.
        """
        df_sc = self.state_code_stats(df)
        df_method = self.method_stats(df)
        spiders = self.spider_stats(df)
        fig, buttons = go.Figure(), list()
        fig.add_trace(
            go.Pie(
                values=spiders["Count"].to_list(),
                labels=spiders["crawlers"].to_list(),
                hole=0.35,
                textinfo="percent",
            )
        )
        buttons.append(self.add_spider_button(spiders, 'Count', 'crawlers', 'Detekovaní boti'))
        buttons.append(self.add_spider_button(spiders, 'Transfer Volume', 'crawlers', 'Prenesený objem'))
        buttons.append(self.add_spider_button(df_sc, 'Count', 'State Code', 'Stavové kódy žiadostí'))
        buttons.append(self.add_spider_button(df_method, 'Count', 'Method', 'Početnosť HTTP metód'))
        fig.update_layout(
            template="plotly",
            showlegend=True,
            height=450,
            updatemenus=[
                {
                    "buttons": buttons,
                    "direction": "down",
                    "showactive": True,
                    "x": 0.68,
                    "y": 1.25,
                }
            ],
            margin=dict(r=10, l=10, t=30, b=15)
        )
        self.figures["crawlers"] = fig


    def user_agent_stats(self, df):
        """
        Ziskanie typov operacneho systemu, prehliadaca a zariadenia z 'User Agent' pola.
        """
        df_os = df.groupby(['User Agent']).sum().reset_index()
        df_os['OS'] = df_os["User Agent"].swifter.apply(lambda x: parse(x).os.family)
        df_os['Browser'] = df_os["User Agent"].swifter.apply(lambda x: parse(x).browser.family)
        df_os['Device'] = df_os["User Agent"].swifter.apply(lambda x: parse(x).device.family)
        df_os.replace({'Other' : 'Neznáme'}, inplace=True)
        return df_os
    

    def add_user_agent_button(self, df, y, x, label):
        """
        Nastavenie tlačidla pre prepínanie medzi grafmi obsahujucimi statistiky medzi uzivatelskymi agentami.
        """
        return dict(
            method='restyle',
            label = label,
            args = [
                {
                    'y' : [df[y].to_list()],
                    'x' : [df[x].to_list()],
                    'text': [df[x].to_list()],
                    'visible' : True,
                }
            ]
        )

    def make_url_stats(self, df):
        """
        Zobrazenie TOP 15 najčastejšie vyskytovaných URL adries.
        """
        url_df = df[['URL']].copy()
        url_df.loc[:, 'Count'] = 1
        url_stats = url_df.groupby(['URL']).sum().reset_index().nlargest(15, 'Count')
        fig = go.Figure()
        fig.add_trace(
            go.Bar(
                y=url_stats['URL'],
                x=url_stats['Count'],
                textposition="auto",
                orientation="h",
            )
        )
        fig.update_layout(
            template='plotly_white',
            height=600,
            width=820,
            showlegend=False,
            title='TOP 15 URL adresy',
            margin_pad=10,
            title_x=0.15,
            title_y=0.88,
            xaxis={'title': 'Početnosť'},
            yaxis={'categoryorder':'total ascending', 'title' : 'Adresa'},
        )
        self.figures['os'] = fig

    def make_user_agent_stats(self, df):
        """
        Ziskanie najcastejsie vyuzivanych operacnych systemov/zariadeni a vyhladavacov ziskanych s pola 'User Agent'
        """
        df_os = df[["crawlers", "Transfer Volume", "User Agent"]].copy()
        df_os.loc[:, "Count"] = 1
        ua = self.user_agent_stats(df_os.loc[df_os["crawlers"] == False, ["User Agent", "Count"]])
        stats_os = ua[['OS', 'Count']].groupby(['OS']).sum().reset_index().nlargest(10, 'Count')
        stats_device = ua[['Device', 'Count']].groupby(['Device']).sum().reset_index().nlargest(10, 'Count')
        stats_browser = ua[['Browser', 'Count']].groupby(['Browser']).sum().reset_index().nlargest(10, 'Count')
        fig, buttons = go.Figure(), list()
        fig.add_trace(
            go.Bar(
                y=stats_os["OS"].to_list(),
                x=stats_os["Count"].to_list(),
                text=stats_os["Count"].to_list(),
                textposition="auto",
                orientation="h",
            )
        )
        buttons.append(self.add_user_agent_button(stats_os, 'OS', 'Count', 'Operačné systémy'))
        buttons.append(self.add_user_agent_button(stats_device, 'Device', 'Count', 'Zariadenia'))
        buttons.append(self.add_user_agent_button(stats_browser, 'Browser', 'Count', 'Prehliadače'))
        fig.update_layout(
            template="plotly_white",
            height=600,
            width=820,
            showlegend=False,
            title="TOP 10 'Uživateľský Agent'",
            title_x=0.16,
            title_y=0.88,
            margin_pad=10,
            xaxis={'title' : 'Počet'},
            yaxis={'categoryorder':'total ascending', 'title' : 'Typ'},
            updatemenus=[
                {
                    "buttons": buttons,
                    "direction": "down",
                    "showactive": True,
                    "x": 0.65,
                    "y": 1.1,
                }
            ],
        )
        self.figures["os"] = fig
        