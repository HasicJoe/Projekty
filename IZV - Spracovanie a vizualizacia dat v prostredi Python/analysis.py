#!/usr/bin/env python3.9
# coding=utf-8
from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import os

# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

""" Ukol 1:
načíst soubor nehod, který byl vytvořen z vašich dat. Neznámé integerové hodnoty byly mapovány na -1.

Úkoly:
- vytvořte sloupec date, který bude ve formátu data (berte v potaz pouze datum, tj sloupec p2a)
- vhodné sloupce zmenšete pomocí kategorických datových typů. Měli byste se dostat po 0.5 GB. Neměňte však na kategorický typ region (špatně by se vám pracovalo s figure-level funkcemi)
- implementujte funkci, která vypíše kompletní (hlubkou) velikost všech sloupců v DataFrame v paměti:
orig_size=X MB
new_size=X MB

Poznámka: zobrazujte na 1 desetinné místo (.1f) a počítejte, že 1 MB = 1e6 B. 
"""


def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    """ 
    Reads a locally stored acident stats file and saves it as a dataframe.

    Attributes:
        filename    Specifies the location of the accident statistics file.
        verbose     If 'True', writes size of dataframe in MB before and after 
                    the optimalisation.
    """
    df = pd.read_pickle(filename)
    if verbose:
        size = round(df.memory_usage(deep=True).sum() / 1048576, 1)
        print(f"orig_size={size} MB")
    # Add column to store date as type 'datetime64'
    df["date"] = df["p2a"].astype("datetime64")
    keys = df.keys()
    keys = keys[: len(keys) - 2]
    df_types = list(dict(df.dtypes[:len(keys)]).values())
    new_types = list()
    # Reducing dataframe size - objects to category, int to smaller ints
    for key, df_type in zip(keys, df_types):
        if str(df_type) == "object":
            new_types.append("category")
        elif str(df_type) == "int64":
            min_limit, max_limit = abs(df[key].min()), abs(df[key].max())
            if min_limit > max_limit:
                max_v = min_limit
            else:
                max_v = max_limit
            if df[key].min() > -1:
                if max_v < 256:
                    new_types.append("uint8")
                elif max_v < 65536:
                    new_types.append("uint16")
                elif max_v < 4294967296:
                    new_types.append("uint32")
                else:
                    new_types.append("uint64")
            else:
                if max_v < 128:
                    new_types.append("int8")
                elif max_v < 32768:
                    new_types.append("int16")
                elif max_v < 2147483648:
                    new_types.append("int32")
                else:
                    new_types.append("int64")
        else:
            new_types.append(str(df_type))
    set_types = list(set(new_types))
    set_types.remove("float64")
    for new_type in set_types:
        columns = [k for k, t in zip(keys, new_types) if t == new_type]
        df[columns] = df[columns].astype(new_type)
    if verbose:
        size = round(df.memory_usage(deep=True).sum() / 1048576, 1)
        print(f"new_size={size} MB")
    return df


# Ukol 2: počty nehod v jednotlivých regionech podle druhu silnic

def plot_roadtype(df: pd.DataFrame, fig_location: str = None,
                  show_figure: bool = False):
    """ 
    Creates a graph (for four optional regions) with the number of accidents
    in each region by type of road.

    Attributes:
        df            Pandas dataframe obtained by the get_dataframe function.
        fig_location  If set, the graph image is saved to the path
                      specified by this argument.
        show_figure   If set, the graph is displayed in the window.
    """
    regions = ["JHM", "KVK", "JHC", "PLK"]
    columns = ['p21', 'region']
    road_types = {
        0: "Žiadna z uvedených",
        1: "Dvojpruhová komunikácia",
        2: "Trojpruhová komunikácia",
        3: "Štvorpruhová komunikácia",
        4: "Štvorpruhová koumunikácia",
        5: "Viacpruhová komunikacia",
        6: "Rýchlostná komunikácia",
    }
    df['p21'] = df['p21'].replace(road_types)
    df_roadtype = df.loc[df['region'].isin(regions), columns]
    df_roadtype['Počet nehôd'] = 0
    df_roadtype = df_roadtype.groupby(columns).agg({"Počet nehôd": "count"}).reset_index()
    sns.set_style('darkgrid')
    fig, axes = plt.subplots(2, 3, figsize=(14, 8.5))
    r, c = [0, 0, 0, 1, 1, 1], [0, 1, 2, 0, 1, 2]
    for i in range(6):
        df_plt = df_roadtype.iloc[(i*4):(i*4+4), :]
        title = "".join(set(df_plt['p21']))
        sns.barplot(data=df_plt, x="region", y="Počet nehôd", ax=axes[r.pop(0), c.pop(0)]).set_title(title)
    plt.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


# Ukol3: zavinění zvěří
def plot_animals(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ 
    Creates a graph (for four optional regions) of accidents caused by the avoidance of forest animals.

    Attributes:
        df              Pandas dataframe obtained by the get_dataframe function.
        fig_location    If set, the graph image is saved to the path specified by this argument.
        show_figure     If set, the graph is displayed in the window.
    """
    regions = ["JHM", "KVK", "JHC", "PLK"]
    columns = ['date', 'region', 'p10']
    df_animals = df.loc[(df['region'].isin(regions)) & (df['p58'] == 5), columns]
    p10_values = list(set(df['p10']))
    known_causes = [1, 2, 4]
    causes = {"Vodič": known_causes[:-1], "Zver": [known_causes[-1]], "Iné": None}
    causes["Iné"] = [item for item in p10_values if item not in known_causes]
    for k, v in causes.items():
        df_animals['p10'] = df_animals['p10'].replace(v, k)
    # Set as categoric type to allow counting zero occurences
    df_animals['Zavinenie'] = df_animals['p10'].astype('category')
    df_animals['Mesiac'] = df_animals['date'].dt.month
    df_animals = df_animals.loc[df_animals['date'].dt.year < 2021, ['Mesiac', 'region', 'Zavinenie']]
    df_animals['Počet nehôd'] = 0
    # Calculate number of accidents
    df_animals = df_animals.groupby(['Mesiac', 'region', 'Zavinenie']).agg({"Počet nehôd": "count"}).reset_index()
    sns.set_style('darkgrid')
    fig, axes = plt.subplots(2, 2, figsize=(14, 8.5))
    for i, region in enumerate(regions):
        df_animals_region = df_animals.loc[df_animals['region'] == region, :]
        sns.barplot(x='Mesiac', y='Počet nehôd', hue='Zavinenie', data=df_animals_region, ax=axes[(i//2) % 2, i % 2], palette="hls")
        legend, handler = axes[(i//2) % 2, i % 2].get_legend_handles_labels()
        axes[(i//2) % 2, i % 2].get_legend().remove()
        axes[(i//2) % 2, i % 2].set_title(f"Kraj: {region}")
    fig.legend(legend, handler, loc='right')
    plt.tight_layout()
    plt.subplots_adjust(right=0.92)
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()
  

# Ukol 4: Povětrnostní podmínky
def plot_conditions(df: pd.DataFrame, fig_location: str = None,
                    show_figure: bool = False):
    """ 
    Creates a line graph (for four optional regions) of the number of 
    accidents in different weather conditions.

    Attributes:
        df            Pandas dataframe obtained by the get_dataframe function.
        fig_location  If set, the graph image is saved to the path specified
                      by this argument.
        show_figure   If set, the graph is displayed in the window.
    """
    regions = ["MSK", "KVK", "PHA", "PLK"]
    columns = ['date', 'region', 'p18']
    df_conditions = df.loc[(df['region'].isin(regions)) & (df['p18'] != 0), columns]
    df_conditions['Počet'] = 0
    conditions = {
        1: "Nesťažené",
        2: "Hmla",
        3: "Mrholenie",
        4: "Dažď",
        5: "Sneženie",
        6: "Námraza",
        7: "Vietor",
    }
    df_conditions['p18'] = df_conditions['p18'].replace(conditions)
    df_conditions = pd.pivot_table(df_conditions, index=['region', 'date'], values='Počet', fill_value=0, columns='p18', aggfunc='count').reset_index()
    start_date = pd.Timestamp(year=2016, month=1, day=1)
    end_date = pd.Timestamp(year=2021, month=1, day=1)
    labels = [f'1/{year}' for year in range(2016, 2022)]
    x = [0, 12, 24, 36, 48, 60]
    sns.set_style('darkgrid')
    fig, axes = plt.subplots(2, 2, figsize=(14, 8.5))
    for i, region in enumerate(regions):
        df_region = df_conditions.loc[(df_conditions['region'] == region) & ((start_date < df_conditions['date']) & (df_conditions['date'] < end_date)), :]
        df_region = df_region.set_index('date').resample('M').sum().reset_index()
        df_region['date'] = df_region['date'].dt.strftime('%Y/%m')
        df_graph = df_region.pivot_table(index='date', values=list(conditions.values()))
        sns.lineplot(data=df_graph, ax=axes[(i//2) % 2, i % 2], dashes=False)
        axes[(i//2) % 2, i % 2].set_title(f'Kraj: {region}')
        axes[(i//2) % 2, i % 2].set_xticks(x)
        axes[(i//2) % 2, i % 2].set_xticklabels(labels, rotation=45)
        legend, handler = axes[(i//2) % 2, i % 2].get_legend_handles_labels()
        axes[(i//2) % 2, i % 2].get_legend().remove()
        axes[(i//2) % 2, i % 2].set_ylabel('Počet nehôd')
        axes[(i//2) % 2, i % 2].set_xlabel('')
    fig.legend(legend, handler, loc='right')
    plt.tight_layout()
    plt.subplots_adjust(right=0.89)
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni ¨
    # funkce.
    df = get_dataframe("accidents.pkl.gz", True)
    plot_roadtype(df, fig_location="01_roadtype.png", show_figure=True)
    plot_animals(df, "02_animals.png", True)
    plot_conditions(df, "03_conditions.png", True)
