#!/usr/bin/env python3.9

import pandas as pd
import seaborn as sns
import numpy as np
from matplotlib import pyplot as plt
import argparse
import sys
import random


def check_validity(df_m, df_b, df_0_m, df_0_b):
    
    df_b['Unemployment'] = df_m['Unemployment'] * 100
    df_b['Immediate inflation'] = 1.0 + df_b['Immediate inflation'] 
    df_b = df_b.groupby(df_b.index // 12).prod()
    df_b.insert(0, 'time', np.arange(0, len(df_b), 1))
    df_b['Immediate inflation'] = (-1.0 + df_b['Immediate inflation'] ) * 100  

    df_0_b['Unemployment'] = df_0_m['Unemployment'] * 100
    df_0_b['Immediate inflation'] = 1.0 + df_0_b['Immediate inflation'] 
    df_0_b = df_0_b.groupby(df_0_b.index // 12).prod()
    df_0_b.insert(0, 'time', np.arange(0, len(df_0_b), 1))
    df_0_b['Immediate inflation'] = (-1.0 + df_0_b['Immediate inflation'] ) * 100  

    sns.set_style('darkgrid')
    g = sns.scatterplot(data=df_b, y='Immediate inflation', x='Unemployment')
    #g = sns.scatterplot(data=df_0_b, y='Immediate inflation', x='Unemployment')
    plt.title('Philipsova krivka')
    plt.xlabel('Nezamestnanosť [%]')
    plt.ylabel('Inflácia [%]')
    plt.tight_layout()
    plt.savefig("exp1philips.png")
    plt.show()
    
    df_0_b = df_0_b.iloc[708//12: 1200, :]
    df_0_b['time'] = df_0_b['time'] - (64)

    df_b = df_b.iloc[708//12: 1200, :]
    df_b['time'] = df_b['time'] - (64)
    g = sns.lineplot(data=df_0_b, y='Immediate inflation', x='time')
    g = sns.lineplot(data=df_b, y='Immediate inflation', x='time')
    plt.legend(['priebeh inflácie modelu bez zásahu', 'priebeh inflácie s lockdownom'])
    plt.title('Výška inflácie v priebehu simulácie')
    plt.xlabel('Čas [roky]')
    plt.ylabel('Inflácia [%]')
    plt.tight_layout()
    plt.savefig("exp1inflation.png")
    plt.show()

def exp_2(df_m, df_b):
    sns.set_style('darkgrid')
    df_b = df_bank.groupby(df_b.index // 12).prod()
    df_b.insert(0, 'time', np.arange(0, len(df_b), 1))
    df_b['Immediate inflation'] = (-1.0 + df_b['Immediate inflation'] ) * 100  
    g = sns.lineplot(data=df_b, y='Immediate inflation', x='time')
    plt.show()
    df_m.insert(0, 'time', np.arange(0, len(df_m), 1))
    df_production = df_m.loc[(df_m['time'] > 790) & (df_m['time'] < 1000), ['time', 'Sum production']]
    df_e = df_m.loc[(df_m['time'] > 790) & (df_m['time'] < 1000) , ['time', 'Employment']]
    g = sns.lineplot(data=df_e, y='Employment', x='time')
    plt.show()
    g = sns.lineplot(data=df_production, y='Sum production', x='time')
    plt.show()


def exp_3():
    pass
    

if __name__ == "__main__":
    df_model = pd.read_csv('model.data', sep=';')
    df_bank = pd.read_csv('bank.data', sep=';')

    df_no_crisis_model = pd.read_csv('model_0.data', sep=';')
    df_no_crisis_bank =  pd.read_csv('bank_0.data', sep=';')
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument('-e', action="store", type=int, help="Spec. N of experiment")
    args = parser.parse_args()
    if args.e == 1:
        check_validity(df_model, df_bank, df_no_crisis_model, df_no_crisis_bank)
    elif args.e == 2:
        #check_validity(df_model, df_bank)
        exp_2(df_model, df_bank)
    elif args.e == 3:
        exp_3()
            

