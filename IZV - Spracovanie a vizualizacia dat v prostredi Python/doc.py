import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def make_motocycle_df(filename):
    """
    Reads a locally stored accident stats and filter only motocycle records.
    Attributes:
        filename    Specifies the location of the accident statistics file.
    """
    df = pd.read_pickle(filename)
    df["date"] = df["p2a"].astype("datetime64")
    return df.loc[(df['p44'] == 1) | (df['p44'] == 2), :]


def acc_in_out_village_stats(df_plot, year):
    """
    Prints to stdout stats of motocycle accidents in/outside the village.
    Attributes:
        df_plot     Resampled pandas DataFrame
        year        Specifies the year from the range <2017, 2020>
    """
    total_motocycle_acc = df_plot['v obci'].sum() + df_plot['mimo obce'].sum()
    print(f"Total morotcycle accident stats for the year {year} = {total_motocycle_acc}")
    df_plot['date'] = df_plot['date'].astype("datetime64")
    # Resample from M -> to Q
    df_q = df_plot.set_index('date').resample('Q').sum().reset_index()
    q = ['Q1', 'Q2', 'Q3', 'Q4']
    df_q['Kvartal'] = q
    df_q['celkovo'] = df_q['v obci'] + df_q['mimo obce']
    # Calc percentage 
    df_q['kvartal/rok'] =  round(df_q['celkovo'] / total_motocycle_acc * 100, 2)
    df_q['v obci perc'] = round(df_q['v obci'] / (df_q['v obci'] + df_q['mimo obce']) * 100, 2) 
    df_q['mimo obce perc'] = round(df_q['mimo obce'] / (df_q['v obci'] + df_q['mimo obce']) * 100, 2) 
    print(f"===== Qarterly morotcycle accident stats for the year {year} =====")
    cols = ['Kvartal', 'v obci', 'mimo obce', 'v obci perc', 'mimo obce perc', 'celkovo', 'kvartal/rok']
    df_display = df_q.loc[:, cols]
    print(df_display.to_string(index=False))
    print('===================================================================')


def acc_in_out_village_visual(ax, year, date_series):
    """
    Adds title & manages ticks to the specific ax which showing accidents in or outside the village in specific year.
    Attributes:
        ax              Axes object
        year            Specifies the year from the range <2017, 2020>
        date_series     DateTime series resampled to Months
    """
    ax.set_title(f'Počet nehôd motocyklistov v obci/mimo obce za rok {year}')
    ax.set_xticks(range(12))
    ax.set_xticklabels(date_series, rotation=60)
    return ax


def acc_in_out_village(df):
    """
    Analysis of mororcycle accidents in/outside village for the years 2017 to 2020.
    Attributes:
        df      pandas DataFrame with motocycle records
    """
    df_acc = df.loc[(df['date'].dt.year >= 2017) & (df['date'].dt.year <= 2020), ['date', 'p5a']]
    df_acc['v obci'] = np.where(df_acc['p5a'] == 2, 1, 0)
    df_acc['mimo obce'] = np.where(df_acc['p5a'] == 1, 1, 0)
    df_acc = df_acc.drop(columns=['p5a'], axis=1)     
    sns.set_style('darkgrid')
    fig, ax = plt.subplots(2, 2, figsize=(16, 8.5))
    r, c = [0, 0, 1, 1], [0, 1, 0, 1]
    print(f"Task: Motocycle accidents in/outside village")
    for year in [2017, 2018, 2019, 2020]:
        df_acc_year = df_acc.loc[df_acc['date'].dt.year == year, :]
        df_resample = df_acc_year.set_index('date').resample('M').sum().reset_index()
        df_resample['date'] = df_resample['date'].dt.strftime('%Y/%m')
        curr_r, curr_c = r.pop(0), c.pop(0)
        sns.lineplot(data=df_resample, ax=ax[curr_r, curr_c], dashes=False, legend=True)
        ax[curr_r, curr_c] = acc_in_out_village_visual(ax[curr_r, curr_c], year, df_resample['date'])
        acc_in_out_village_stats(df_resample, year)
    plt.tight_layout()
    plt.savefig('fig01.png')
    print("Graph for this task saved as fig01.png")


def moto_acc_stats(df):
    """
    Prints to stdout stats of main causes of motocycle accidents.
    Attributes:
        df      pandas DataFrame with motocycle records
    """
    df_sum = df.groupby('p12').agg({"Počet nehôd": "sum"}).reset_index()
    print(f"\nTask: Main causes of motocyclist accidents")
    tot_acc = df_sum['Počet nehôd'].sum()
    print(f"Total number of motocycle accidents for the <2017-2020> = {tot_acc}")
    df_sum['Percentualny podiel'] = round((df_sum['Počet nehôd']/ tot_acc) * 100, 2)
    print(df_sum.rename(columns={'p12': "Hlavná príčina"}).to_string(index=False))


def moto_accidents_causes(df):
    """
    Analysis of mororcycle accidents with known main causes for the years 2017 to 2020.
    Attributes:
        df      pandas DataFrame with motocycle records
    """
    df_acc = df.loc[(df['date'].dt.year >= 2017) & (df['date'].dt.year <= 2020), ['date', 'p12']]
    causes = {100 :'Nezavinená vodičom'}
    [causes.update({i : 'Neprimeraná rýchlosť jazdy'}) for i in range(201, 210)]
    [causes.update({i : 'Nesprávne predbiehanie'}) for i in range(301, 312)]
    [causes.update({i : 'Nedanie prednosti v jazde'}) for i in range(401, 415)]
    [causes.update({i : 'Nesprávny spôsob jazdy'}) for i in range(501, 517)]
    [causes.update({i : 'Technická závada vozidla'}) for i in range(601, 616)]
    df_acc['p12'] = df_acc['p12'].replace(causes)
    sns.set_style('darkgrid')
    df_acc['rok'] = df_acc['date'].dt.year
    df_acc = df_acc.drop(columns=['date'], axis=1)     
    df_acc['Počet nehôd'] = 0
    df_acc = df_acc.groupby(['rok', 'p12']).agg({"Počet nehôd": "count"}).reset_index()
    fig, ax = plt.subplots(figsize=(16, 9.5))
    sns.barplot(data=df_acc, x="rok", y="Počet nehôd", hue='p12', ax=ax)
    ax.set_title("Hlavné príčiny nehôd motocyklov v rokoch 2017-2020")
    plt.legend(bbox_to_anchor=(1.01, .5), borderaxespad=0, title="Hlavné príčiny nehody")
    plt.tight_layout()
    plt.savefig('fig02.png')
    moto_acc_stats(df_acc)
    print("Graph for this task saved as fig02.png")


def weather_print_header():
    """Prints header for task 3 -- analysis of how weather conditions affect excessive driving speed."""
    print('Podmienky;Počet obetí;Počet nehôd;Obeť/nehoda[%]')


def calc_weather_stats(df):
    """
    Calculate how weather conditions affect specific weather condition
    Attributes:
        df pandas sub DataFrame with specific weather condition
    """
    df['Počet obetí'] = 0
    df['Počet obetí'] = df['p13a'] * df['Počet nehôd']
    cause = ''.join(set(df['p18']))
    n_of_victims = df['Počet obetí'].sum()
    n_of_accidents = df['Počet nehôd'].sum()
    victim_p_acc = round(n_of_victims / n_of_accidents * 100, 2)
    print(f"{cause};{n_of_victims};{n_of_accidents};{victim_p_acc}")
 

def moto_weather(df):
    """
    Analysis of how weather conditions affect excessive driving speed.
    Attributes:
        df      pandas DataFrame with motocycle records
    """
    df_w = df.loc[(df['p12'] >= 200) & (df['p12'] < 210), ['p18', 'p13a']]
    # other conditions are ignored due to low incidence 
    conditions = {
        1 : "Nesťažené",
        3 : "Mrholenie",
        4 : "Dážď",
    }
    df_w = df_w.loc[(df_w['p18'] == 1) | (df_w['p18'] == 3) | (df_w['p18'] == 4), :]
    df_w['p18'] = df_w['p18'].replace(conditions)
    df_w['Počet nehôd'] = 0
    df_w = df_w.groupby(['p18', 'p13a']).agg({"Počet nehôd" : "count"}).reset_index()
    conds = df_w['p18'].unique()
    print('Task: Analysis how weather affect excesive driving speed.')
    weather_print_header()
    for cond in conds:
        calc_weather_stats(df_w.loc[df_w['p18'] == cond, :].copy())
       

if __name__ == '__main__':
    df = make_motocycle_df(filename='accidents.pkl.gz')
    acc_in_out_village(df)
    moto_accidents_causes(df)
    moto_weather(df)