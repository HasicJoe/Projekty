"""
Autor: Samuel Valaštín
Dátum : 1.3.2022
Súbor funkcii pre dolovanie znalostí z predspracovaných logov a naslednu vizualizaciu ziskanych znalosti.
    Zhlukova analyza.
    Dolovanie asociačných pravidiel.
    Dolovanie sekvenčných vzorov.
    Kombinácia zhlukovania v kombinacii bud s dolovanim asociačných pravidiel alebo sekvenčných vzorov.

"""


from flask import Blueprint, request, jsonify
import json, math, time
from .save import g_Drive
import numpy as np
import pandas as pd
import plotly
import plotly.express as px
import plotly.graph_objects as go
import sklearn.cluster as cluster
from mlxtend.preprocessing import TransactionEncoder
from mlxtend.frequent_patterns import apriori, association_rules, fpgrowth
from prefixspan import PrefixSpan


mine = Blueprint('mine', __name__)


class Scenario:
    CLUSTERING = '1'
    CLUSTERING_PLUS_ASSOCIATION_RULES = '2'
    ASSOCIATION_RULES = '3'
    SEQUENCE_PATTERN = '4'
    CLUSTERING_PLUS_SEQUENCE_PATTERN = '5'

    CLUSTER_PLOT_LABELS = {
        "SessionDuration" : "Dĺžka sedenia (s)",
        "PathLen" : "Počet navštívených stránok",
        "Average time pp" : "Priemerný čas na stránku (s)"
    }

    ASSOCIATION_RULES_HEADER = {
        'rules' : [],
        'support' : [],
        'confidence' : [],
        'lift' : []
    }

def load_dataset(filename):
    """
    Nacitanie ulozeneho dataframu vo formate pickle z google drive.
    """
    return g_Drive().get_dataframe(filename)


def process_form_arguments(form_data):
    """
    Ziskanie zaslanych argumentov z GUI.
    """
    data = dict()
    for k, v in form_data.items():
        data[k] = v
    return data


def normalize_fit_data(df):
    """
    Normalizacia dat ziskanych z prieskumnej analyzy do rozsahu <0, 1>
    """
    sd_min, sd_max = df['SessionDuration'].min(), df['SessionDuration'].max()
    pl_min, pl_max = df['PathLen'].min(), df['PathLen'].max()
    avpp_min, avpp_max = df['Average time pp'].min(), df['Average time pp'].max()
    df['SD_N'] =  (df['SessionDuration'] - sd_min) / (sd_max - sd_min)
    df['PL_N'] =  (df['PathLen'] - pl_min) / (pl_max - pl_min)
    df['AVPP_N'] = (df['Average time pp'] - avpp_min) / (avpp_max - avpp_min)


def clustering(data, df):
    """
    Zhlukova analyza na zaklade zvoleneho algoritmu pre zhlukovanie specifikovaneho parametrom 'data'.
    """
    try:
        normalize_fit_data(df)
        if data['cluster_algo'] == 'Birch':
            if data['b_clusters'] == '0':
                data['b_clusters'] = None
            else:
                data['b_clusters'] = int(data['b_clusters'])

            db = cluster.Birch(
                threshold=float(data['treshold']),
                branching_factor = int(data['branching-factor']),
                n_clusters = data['b_clusters']
            ).fit(df[['SD_N', 'PL_N', 'AVPP_N']].to_numpy())
        elif data['cluster_algo'] == "K-means":

            db = cluster.KMeans(
                n_clusters = int(data['k_clusters'])
            ).fit(df[['SD_N', 'PL_N', 'AVPP_N']].to_numpy())

        elif data['cluster_algo'] == 'DBSCAN':
            db = cluster.DBSCAN(
                eps=float(data['db_eps']), 
                min_samples=int(data['db_samples'])
            ).fit(df[['SD_N', 'PL_N', 'AVPP_N']].to_numpy())
        df['cluster'] = db.labels_
    except:
        return False
    return True


def add_table_button(name, visib_index, trace_len):
    """
    Pridanie tlačidla pre prepínanie stop medzi tabulkami zhlukov.
    """
    return dict(
        method="restyle",
            label=name,
            args=[
                {
                    'visible' : [True if visib_index == n else False for n in range(trace_len)],
                }
            ],
        )


def make_clustering_table(df):
    """
    Vytvorenie tabulky pre zobrazenie informacii o zhlukoch.
    """
    fig, button = go.Figure(), list()
    grouped_clusters = df.groupby(['cluster'])
    total_sessions = df.shape[0]
    clusters = sorted(list(df['cluster'].unique()))
    for index, cluster in enumerate(clusters):
        cluster_df = grouped_clusters.get_group(cluster).copy().reset_index()
        table_data = {
            "<b>Počet uživteľských sedení</b>" : cluster_df.shape[0],
            "<b>Percentuálny podiel sedení</b>" : f"{round((cluster_df.shape[0] / total_sessions) * 100, 2)} %",
            "<b>Maximálna dĺžka sedenia (s)</b>" : round(cluster_df['SessionDuration'].max(), 2),
            "<b>Priemerná dĺžka sedenia (s)</b>" : round(cluster_df['SessionDuration'].mean(), 2),
            "<b>Minimálna dĺžka sedenia (s)</b>" : round(cluster_df['SessionDuration'].min(), 2),
            "<b>Maximálna dĺžka cesty</b>" : round(cluster_df['PathLen'].max(), 2),
            "<b>Priemerná dĺžka cesty</b>" : round(cluster_df['PathLen'].mean(), 2),
            "<b>Minimálna dĺžka cesty</b>" : round(cluster_df['PathLen'].min(), 2),
            "<b>Maximálny priemerný čas na stránku (s)</b>" : round(cluster_df['Average time pp'].max(), 2),
            "<b>Priemerný čas na stránku (s)</b>" : round(cluster_df['Average time pp'].mean(), 2),
            "<b>Minimálny priemerný čas na stránku (s)</b>" : round(cluster_df['Average time pp'].min(), 2),
        }
        cells_values = [list(table_data.keys()), list(table_data.values())]
        fig.add_trace(
            go.Table(
                columnwidth = [300, 100],
                cells=dict(
                    values=cells_values,
                    align='left',
                ),
                visible = 'legendonly' if index >= 1 else True, 
            )
        )
        button.append(add_table_button(f'<b>Zobraziť informácie - zhluk {index+1} </b>', index, len(clusters)))
    """
    Zbavenie sa zahlavia tabulky a uprava vzhladu
    """
    fig.for_each_trace(lambda x: x.update(header_fill_color = 'rgba(0,0,0,0)'))
    fig.update_layout(
        template="seaborn",
        height=650,
        width=550,
        updatemenus=[
            {
                "buttons": button,
                "direction": "down",
                "showactive": True,
                "x": 0.67,
                "y": 1.10,
            }
        ],
        margin=dict(r=0, l=140,t=250,b=0),
    )
    return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def make_clustering_plot(df):
    """
    Vytvorenie grafu pre zobrazenie zhlukov.
    """
    df = df.rename(columns=Scenario.CLUSTER_PLOT_LABELS)
    """
    Konverzia na string a nastavenie jednoduchej legendy
    """
    df['cluster'] = df['cluster'] + 1
    df['cluster'] = 'Zhluk ' + df['cluster'].astype(str)
    df = df.sort_values(['cluster'])
    fig = px.scatter_3d(
        df, x='Počet navštívených stránok',
        y='Dĺžka sedenia (s)',
        z='Priemerný čas na stránku (s)', 
        color='cluster', 
        opacity=0.6, 
        labels={'cluster' : 'Cluster'},
        width=1080,
        height=850,
    )
    fig.update_traces(marker_size=5)
    fig.update_layout(template='plotly_white', legend=dict(y=0.53, x=0.85, title_text='<b>Legenda</b>', font=dict(size=16)))
    fig.update_layout(margin=dict(r=0, l=0,t=20,b=20))
    return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def transaction_preprocess(df):
    """
    Transformácia ciest uživateľských sedení na transakcie.
    """
    paths = df['Path'].tolist()
    encoder = TransactionEncoder()
    transactions = encoder.fit(paths).transform(paths)
    transactions = transactions.astype('int')
    return pd.DataFrame(transactions, columns=encoder.columns_)


def add_percentage(values):
    """
    Pridanie % pre zobrazenie asociacnych pravidiel.
    """
    values = [f"{str(value)} %" for value in values]
    return values


def clear_dictionary_data(data_d):
    """
    Vycistenie hodnot klucov slovnika pre dalsie pouzitie.
    """
    for k in data_d.keys():
        data_d[k].clear()
    return data_d


def make_rules_table(as_rules_df, cluster = None, cluster_fig = None):
    """
    Vizualizacia ziskanych asociacnych pravidiel prostrednictvom tabulky.
    """
    table_data = {
        'rules' : [],
        'support' : [],
        'confidence' : [],
        'lift' : []
    }
    for as_rule in as_rules_df.itertuples(index=False):
        antecedents = list(as_rule.antecedents)
        consequents = list(as_rule.consequents)
        table_data['rules'].append(f"{', '.join(antecedents)} => {', '.join(consequents)}")
        table_data['support'].append(round(as_rule.support * 100, 2))
        table_data['confidence'].append(round(as_rule.confidence * 100, 2))
        table_data['lift'].append(round(as_rule.lift, 2))
    for k in ['support', 'confidence']:
        table_data[k] = add_percentage(table_data[k])
    if cluster:
        table_values = np.array([table_data[k] for k in ('rules', 'support', 'confidence', 'lift')])
        cluster_fig.add_trace(
            go.Table(
                columnwidth = [1060, 130, 130, 130],
                header=dict(
                    height=30,
                    font_size=14,
                    values=[
                        "<b>Asociačné pravidlo</b>",
                        "<b>Podpora</b>",
                        "<b>Spoľahlivosť</b>",
                        "<b>Lift</b>",
                    ]
                ), 
                cells=dict(
                    values=table_values,
                    align='left',
                    height=30,
                    font_size=13,
                ),
                visible = 'legendonly' if cluster_fig['data'] else True, 
            )
        )
    else:
        fig = go.Figure(
            data=[go.Table(
                columnwidth = [1060, 130, 130, 130],
                header=dict(
                    height=30,
                    font_size=14,
                    values=[
                        "<b>Asociačné pravidlo</b>",
                        "<b>Podpora</b>",
                        "<b>Spoľahlivosť</b>",
                        "<b>Lift</b>",
                    ]
                ),
                cells=dict(
                    values=[
                        table_data["rules"],
                        table_data["support"],
                        table_data["confidence"],
                        table_data["lift"],
                    ],
                    height=30,
                    font_size=13,
                ),
            )])
        fig.update_layout(
            template='plotly_dark', 
            margin=dict(r=50, l=100,t=45,b=45),
            paper_bgcolor='rgba(0,0,0,0)',
            height=1400,
            plot_bgcolor='rgba(0,0,0,0)',
        )
        return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def mine_rules_from_clusters(data, df):
    """
    Dolovanie asociacnych pravidiel po zhlukovani.
    """
    fig, button = go.Figure(), list()
    grouped_clusters = df.groupby(['cluster'])
    clusters = sorted(list(df['cluster'].unique()))
    button_header = list()
    match = False
    for index, cluster in enumerate(clusters):
        cluster_df = grouped_clusters.get_group(cluster).copy().reset_index()
        """
        asociacne pravidla su ziskavane len z tych zhlukov, ktore obsahuju minimalne 10 uzivatelskych sedeni
        """
        if cluster_df.shape[0] >= 10:
            transaction_df = transaction_preprocess(cluster_df)
            rules = mine_association_rules(transaction_df, data['fp-algo-select'], data['support'], data['confidence'])
            if isinstance(rules, pd.DataFrame):
                match = True
                make_rules_table(rules, cluster=True, cluster_fig=fig)
                button_header.append(f"Zobraziť asociačné pravidlá - zhluk {index+1}")
    if not match:
        return False
    for index, header in enumerate(button_header):
        button.append(add_table_button(header, index, len(button_header)))
    fig.update_layout(
        template='plotly_dark', 
        paper_bgcolor='rgba(0,0,0,0)',
        plot_bgcolor='rgba(0,0,0,0)',
        height=650,
        width=1400,
        updatemenus=[
            {
                "buttons": button,
                "direction": "down",
                "showactive": True,
                "x": 0.65,
                "y": 1.10,
            }
        ],
        margin=dict(r=0, l=140,t=100,b=0),
    )
    return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def mine_sequential_patterns(paths, min_support, min_len, frame_len, algorithm, unique_pages):
    """
    Dolovanie skevencnych vzorov bez zhlukovania.
        v buducnosti moznost rozsirenia pomocou vyuzitia inych algoritmov
    """
    if algorithm == 'PrefixSpan':
        integer_keys, i_paths = paths_to_integers(paths, unique_pages)
        prefixspan = PrefixSpan(i_paths)
        prefixspan.minlen = min_len
        patterns = prefixspan.frequent(math.ceil(frame_len * (float(min_support))))
    else:
        return False
    """
    Nepodarilo sa vydolovat sekvencne vzory.
    """
    if len(patterns) == 0:
        return False
    patterns = sorted(patterns, key=lambda x: x[0], reverse=True)
    data = [
        [
            f"{support}/{frame_len}",
            " -> ".join(transfer_integers_to_pages(sequence, integer_keys)),
            round(100 * support / frame_len, 2),
        ]
        for support, sequence in patterns
    ]
    """
    Vizualizacia ziskanych sekvencnych vzorov.
    """
    fig = go.Figure(
        data=[go.Table(
            columnwidth = [120, 1000, 125],
            header=dict(
                height=30,
                font_size=14,
                values=['<b>Podpora</b>', '<b>Sekvenčný vzor</b>', '<b>Početnosť [%]</b>']
            ),
            cells=dict(
                values=[list(i) for i in zip(*data)],
                height=30,
                font_size=14,
                align='left',
            ),
        )]
    )
    fig.update_layout(
        template='plotly_dark', 
        paper_bgcolor='rgba(0,0,0,0)',
        plot_bgcolor='rgba(0,0,0,0)', 
        margin=dict(r=50, l=50,t=32,b=32),
    )
    return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def paths_to_integers(paths, unique_pages):
    """
    Transformacia stranok tvoriacich cesty uzivatelskych sedeni do celociselneho formatu.
    Okrem toho tato funkcia vracia spojujuci slovnik pre nasledne preklopenie celociselnych indexov
    vyskytov stranok na ich retazcovu podobu
    """
    integer_keys = dict(enumerate(unique_pages.flatten(), 1))
    join_dictionary = {v : k for k, v in integer_keys.items()}
    integer_paths = [[join_dictionary[page]for page in path] for path in paths] 
    return integer_keys, integer_paths


def transfer_integers_to_pages(sequence, join_dictionary):
    """
    Transformacia celociselnych oznaceni stranok spat na retazcovu podobu stranok.
    """
    page_sequence = [join_dictionary[item] for item in sequence]
    return page_sequence


def mine_cluster_sequential_patterns(df, min_support, min_len, algorithm):
    """
    Dolovanie sekvencnych vzorov po zhlukovani.
    """
    fig, button, button_header = go.Figure(), list(), list()
    clusters = sorted(list(df['cluster'].unique()))
    match = False
    for cluster in clusters:
        cluster_df = df.loc[df['cluster'] == cluster, :]
        cluster_df_len = cluster_df.shape[0]
        """
        v buducnosti moznost rozsirenia.
        """
        if cluster_df_len >= 10 and algorithm == 'PrefixSpan':
            integer_keys, i_paths = paths_to_integers(cluster_df['Path'].tolist(), cluster_df['Path'].explode().unique())
            ps = PrefixSpan(i_paths)
            ps.minlen = min_len
            patterns = ps.frequent(math.ceil(cluster_df_len * (float(min_support) + 0.01)), closed=True)
            """
            PrexifSpan vracia uzavrete sekvencne vzory vo formate 
                    (37, ['/studium/login.php', 'studium/predmety.php'])
                    ..........
                    (31, ['/studium/login.php', '/studium/index.php'])
                Zoradenie vydolovanych sekvencnych vzorov a nasledna 
                transformacia dat pre zobrazenie do formatu:
                    Podpora
                    Sekvencny vzor
                    Pocetnost
            """
            if len(patterns):
                patterns = sorted(patterns, key=lambda x: x[0], reverse=True)
                data = [
                    [
                        f"{support}/{cluster_df_len}",
                        " -> ".join(transfer_integers_to_pages(sequence, integer_keys)),
                        round(100 * support / cluster_df_len, 2),
                    ]
                    for support, sequence in patterns
                ]
                button_header.append(f"Zobraziť sekvenčné vzory - zhluk {cluster+1}")
                fig.add_trace(
                    go.Table(
                        columnwidth = [120, 1000, 140],
                        header=dict(
                            height=30,
                            font_size=14,
                            values=['<b>Podpora</b>', '<b>Sekvenčný vzor</b>', '<b>Početnosť [%]</b>']
                        ),
                        cells=dict(
                            values=[list(i) for i in zip(*data)],
                            height=30,
                            font_size=14,
                            align='left',
                        ),
                        visible = 'legendonly' if match else True, 
                    )
                )
                match = True
    """
    Nepodarilo sa vydolovat sekvencne vzory
    """
    if not match:
        return False
    for index, header in enumerate(button_header):
        button.append(add_table_button(header, index, len(button_header)))
    fig.update_layout(
        template='plotly_dark', 
        paper_bgcolor='rgba(0,0,0,0)',
        plot_bgcolor='rgba(0,0,0,0)',
        height=650,
        width=1260,
        updatemenus=[
            {
                "buttons": button,
                "direction": "down",
                "showactive": True,
                "x": 0.65,
                "y": 1.10,
            }
        ],
        margin=dict(r=0, l=140,t=100,b=65),
    )
    return json.dumps(fig, cls=plotly.utils.PlotlyJSONEncoder)


def mine_association_rules(transaction_df, freq_items_algo, support, confidence):
    """
    Ziskanie mnoziny frekventovanych vzorov a nasledne dolovanie silnych asociacnych pravidiel.
    """
    if freq_items_algo == 'Apriori':
        frequent_items = apriori(transaction_df, min_support=float(support), use_colnames=True, low_memory=True)
    elif freq_items_algo == 'FP-Growth':
        frequent_items = fpgrowth(transaction_df, min_support=float(support), use_colnames=True)
    if frequent_items.empty:
        return False
    rules = association_rules(frequent_items, metric='confidence', min_threshold=float(confidence))
    if rules.empty:
        return False
    return rules


def catch_invalid_options(user_data):
    """
    Osetrenie chybnych scenarov a neplatnych uzivatelskych poziadaviek pre dolovanie.
    Scenare 1, 2, 5 vyzaduju volbu zhlukovacieho algoritmu z ponuky = BIRCH,DBSCAN,k-Means
    Scenare 2, 3 vyzaduju uzivatelsku volbu algoritmu pre dolovanie frekventovanych poloziek - Apriori, FP-Growth
    Scenare 4, 5 vyzaduju uzivatelsku volbu algoritmu pre dolovanie sekvencnych vzorov - PrefixSpan, BIDE
    """
    if not 'scenario' in user_data:
        return True, 'Nebol zvolený dolovací scenár.'
    if int(user_data['scenario']) in [1, 2, 5] and not 'cluster_algo' in user_data:
        return True, 'Nebol zvolený algoritmus pre zhlukovanie.'
    if int(user_data['scenario']) in [2, 3] and not 'fp-algo-select' in user_data:
        return True, 'Nebol zvolený algoritmus pre dolovanie frekventovaných vzorov'
    if int(user_data['scenario']) in [4, 5] and not 'sp-algo-select' in user_data:
        return True, 'Nebol zvolený algoritmus pre dolovanie sekvenčných vzorov'
    return False, False


@mine.route('/get_knowledge', methods=['POST'])
def get_knowledge():
    """
    Spracovanie scenarov pre dolovanie znalosti z webovych pristupovych logov
    """
    start_debug = time.time()
    data = process_form_arguments(request.form)
    response = dict()
    error, message = catch_invalid_options(data)
    if error:
        return jsonify({'error' : message}), 201
    df = load_dataset(data['processed_file'] + '.pkl')
    if data['scenario'] == Scenario.CLUSTERING:
        """
        zhlukovaci scenar
        """
        done = clustering(data, df)
        if not done:
            response['error'] = 'Nemožno vykonať zhlukovanie. Upravte parametre.'
        else:
            response['cluster_table'] = make_clustering_table(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
            response['cluster'] = make_clustering_plot(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
    elif data['scenario'] == Scenario.ASSOCIATION_RULES:
        """
        dolovanie asociacnych pravidiel bez zhlukovania
        """
        transaction_df = transaction_preprocess(df)
        association_rules_df = mine_association_rules(transaction_df, data['fp-algo-select'], data['support'], data['confidence'])
        if not isinstance(association_rules_df, pd.DataFrame):
            response['error'] = 'Nepodarilo sa vydolovať asociačne pravidlá. Znížte minimálne hodnoty parametrov podpory/dôvery.'
        else:
            response['table'] = make_rules_table(association_rules_df)
    elif data['scenario'] == Scenario.CLUSTERING_PLUS_ASSOCIATION_RULES:
        """
        zhlukovanie a nasledne dolovanie asociacnych pravidiel
        """
        done = clustering(data, df)
        if not done:
            response['error'] = 'Nemožno vykonať zhlukovanie. Upravte parametre.'
        else:
            response['cluster_table'] = make_clustering_table(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
            response['cluster'] = make_clustering_plot(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
            response['cluster_rules'] = mine_rules_from_clusters(data, df)
            if not response['cluster_rules']:
                response['error'] = 'Nepodarilo sa vydolovať asociačne pravidlá. Znížte minimálne hodnoty parametrov podpory/dôvery.'
    elif data['scenario'] == Scenario.SEQUENCE_PATTERN:
        """
        dolovanie sekvencnych vzorov
        """
        response['sequential_pattern'] = mine_sequential_patterns(
            df['Path'].tolist(), 
            data['ps_min_support'], 
            int(data['ps_min_len']), 
            df.shape[0], 
            data['sp-algo-select'],
            df['Path'].explode().unique()
        )
        if not response['sequential_pattern']:
            response['error'] = 'Nepodarilo sa vydolovať sekvenčné vzory. Znížte hodnoty podpory/minimálnej dĺžky sekvencie.'
    elif data['scenario'] == Scenario.CLUSTERING_PLUS_SEQUENCE_PATTERN:
        """
        zhlukovanie a nasledne dolovanie sekvencnych vzorov
        """
        done = clustering(data, df)
        if not done:
            response['error'] = 'Nemožno vykonať zhlukovanie. Upravte parametre.'
        else:
            response['cluster_table'] = make_clustering_table(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
            response['cluster'] = make_clustering_plot(df[['SessionDuration', 'PathLen', 'cluster', 'Average time pp', 'Path']].copy())
            response['sequential_pattern'] = mine_cluster_sequential_patterns(df, data['ps_min_support'], int(data['ps_min_len']), data['sp-algo-select'])
            if not response['sequential_pattern']:
                response['error'] = 'Nepodarilo sa vydolovať sekvenčné vzory. Znížte hodnoty podpory/minimálnej dĺžky sekvencie.'
    response['status'] = 200
    if 'error' in response:
        return jsonify(response), 201
    response['ok'] = 'Dolovanie znalostí úspešne dokončené'
    print(f'Elapsed total: {time.time() - start_debug}')
    return jsonify(response), 200