import tkinter as tk
from tkinter import ttk
from tkinter.ttk import Progressbar,Style
from tkinter import filedialog as fd
import pandas
import numpy as np
import re
from datetime import datetime
from datetime import date
import random
import os
from openpyxl import load_workbook
from openpyxl.worksheet.dimensions import ColumnDimension, DimensionHolder
from openpyxl.utils import get_column_letter
import xlsxwriter
import sys



def parse_people(df,names,months,years,writer):
    list_of_emp = []
    for empl in names:
        name_split = df[df["Meno vodiča"].str.match(empl)==True]
        list_of_emp.append(name_split)
    
    mega_set_mix = []
    for mon in months:
        for emp in list_of_emp:
            split = emp[emp["Dátum výkonu"].str.match(mon)==True]
            if len(split):
                mega_set_mix.append(split)

    meno_vodica = ""
    mesiac = ""
    pocet_stop_rozvoz = 0
    pocet_stop_zvoz = 0
    hmotnost_dor_zas = 0.0
    hmotnost_zvez_zas = 0.0
    pocet_dor_stop = 0
    pocet_zvez_stop = 0
    bar["value"] = 71
    last_dance = []

    for this_emp_this_mon in mega_set_mix:
        for i in this_emp_this_mon.index:
            meno_vodica = this_emp_this_mon["Meno vodiča"][i]
            mesiac = this_emp_this_mon["Dátum výkonu"][i]
            pocet_stop_rozvoz += this_emp_this_mon["počet stopov rozvoz"][i]
            pocet_stop_zvoz += this_emp_this_mon["počet stopov zvoz"][i]
            hmotnost_dor_zas += this_emp_this_mon["hmotnosť doručených objednávok"][i]
            hmotnost_zvez_zas += this_emp_this_mon["hmotnosť zvezených objednávok"][i]
            pocet_dor_stop += this_emp_this_mon["počet doručených stopov"][i]
            pocet_zvez_stop += this_emp_this_mon["počet zvezených stopov"][i]

        if pocet_stop_rozvoz != 0:
            usp_rozvoz = ((pocet_dor_stop / pocet_stop_rozvoz)*100)
        if pocet_stop_zvoz != 0:
            usp_zvoz = ((pocet_zvez_stop / pocet_stop_zvoz)*100)

        sum_rozvoz_zvoz = pocet_stop_rozvoz + pocet_stop_zvoz
        split_mon = re.split("\-",mesiac)
        mes = split_mon[0]+"-"+split_mon[1]
        
        one_row = [meno_vodica,mes,pocet_stop_rozvoz,pocet_stop_zvoz,hmotnost_dor_zas,
        hmotnost_zvez_zas,pocet_dor_stop,pocet_zvez_stop,sum_rozvoz_zvoz,
        round(usp_rozvoz,0),round(usp_zvoz,0)]
        
        last_dance.append(one_row)
        meno_vodica = ""
        pocet_stop_rozvoz = 0
        pocet_stop_zvoz = 0
        hmotnost_dor_zas = 0.0
        hmotnost_zvez_zas = 0.0
        pocet_dor_stop = 0
        pocet_zvez_stop = 0
    
    by_month_emp_df = pandas.DataFrame(last_dance,columns = ["Meno vodiča",
    "Mesiac výkonu","Počet stopov rozvoz", "Počet stopov zvoz","Hmotnosť obj. rozvoz",
    "Hmotnosť obj. zvoz","Počet doručených stopov","Počet zvezených stopov",
    "Σ (Rozvoz + zvoz)","% úsp. rozvoz","% úst. zvoz"])
    
    write_emp(by_month_emp_df,writer)      



def parse_depo(df,days,months,years):

    day_data = {"Deň","Počet rozvozov","Hmotnosť rozvoz","Počet zvozov","Hmotnosť zvoz", "Σ(Rozvoz + zvoz)"}
    hmotnost_rozvozu = 0.0
    hmotnost_zvozu = 0.0
    pocet_zvozov = 0
    pocet_rozvozov = 0
    day_list = []
    month_list = []
    bar["value"] = 25
    for this_day in days:
        for i in df.index:
            if df["Dátum výkonu"][i] == this_day:
                hmotnost_rozvozu += df["hmotnosť doručených objednávok"][i]
                hmotnost_zvozu += df["hmotnosť zvezených objednávok"][i]
                pocet_zvozov += df["počet zvezených stopov"][i]
                pocet_rozvozov += df["počet doručených stopov"][i]
        my_sum = pocet_zvozov + pocet_rozvozov
        any_row = [this_day,pocet_rozvozov, hmotnost_rozvozu,pocet_zvozov,hmotnost_zvozu,my_sum]
        day_list.append(any_row)
        hmotnost_rozvozu = 0.0
        hmotnost_zvozu = 0.0
        pocet_zvozov = 0
        pocet_rozvozov = 0

    depo_frame_day = pandas.DataFrame(day_list,columns = ["Deň","Počet rozvozov","Hmotnosť rozvoz",
    "Počet zvozov","Hmotnosť zvoz", "Σ (Rozvoz + zvoz)"])
    bar["value"] = 30
    new_frame = df.copy()

    month_list = new_frame.values.tolist()
    list_of_kgs_rozvoz = []
    list_of_kgs_zvoz = []
    list_of_kgs_mnths = []
    list_of_months = []
    last_list_mon = []
    ## TODO
    for this_month in months:
        month_split = df[df["Dátum výkonu"].str.match(this_month)==True]
        list_of_months.append(month_split)

    for this_mon in list_of_months:
        for i in this_mon.index:
            mon = this_mon.loc[i]["Dátum výkonu"]
            hmotnost_rozvozu += this_mon.loc[i]["hmotnosť doručených objednávok"]
            hmotnost_zvozu += this_mon.loc[i]["hmotnosť zvezených objednávok"]
            pocet_zvozov += this_mon.loc[i]["počet zvezených stopov"]
            pocet_rozvozov += this_mon.loc[i]["počet doručených stopov"]

        my_sum = pocet_zvozov + pocet_rozvozov
        split_month = re.split("\-",mon)
        mont_fix = split_month[0]+"-"+split_month[1]
        list_of_kgs_rozvoz.append(hmotnost_rozvozu)
        list_of_kgs_zvoz.append(hmotnost_zvozu)
        list_of_kgs_mnths.append(mont_fix)
        
        any_row = [mont_fix,pocet_rozvozov,round(hmotnost_rozvozu,2),
        pocet_zvozov,round(hmotnost_zvozu,2),my_sum]
        
        hmotnost_rozvozu = 0.0
        hmotnost_zvozu = 0.0
        pocet_zvozov = 0
        pocet_rozvozov = 0
        last_list_mon.append(any_row)

        
    depo_frame_month = pandas.DataFrame(last_list_mon,columns = ["Mesiac","Počet rozvozov",
    "Hmotnosť rozvoz","Počet zvozov","Hmotnosť zvoz", "Σ (Rozvoz + zvoz)"])

    bar["value"] = 40

    writer = write_depo(depo_frame_day,depo_frame_month,
    list_of_kgs_rozvoz,list_of_kgs_zvoz,list_of_kgs_mnths)
    return writer


def multiple_dfs(df_list, sh_name, file_name, spaces,col_len,l_rozvoz,l_zvoz,l_months):
    head_day = ["Deň","Počet rozvozov","Hmotnosť rozvoz","Počet zvozov",
    "Hmotnosť zvoz", "Σ (Rozvoz + zvoz)"]
    
    head_month = ["Mesiac","Počet rozvozov","Hmotnosť rozvoz",
    "Počet zvozov","Hmotnosť zvoz", "Σ (Rozvoz + zvoz)"]
    
    writer = pandas.ExcelWriter(file_name,engine='xlsxwriter')
    workbook  = writer.book   
    bar["value"] = 51
    header_fmt = workbook.add_format({'font_name': 'Arial', 'font_size': 10,
    'bold': True,'bg_color': '#303030'})
    row = 0
    
    for dataframe in df_list:
        dataframe.to_excel(writer,sheet_name=sh_name,startrow=row , startcol=0,index=False)
        if row == 0:
            worksheet = writer.sheets[sh_name]
            worksheet.autofilter(row,0,row,col_len)
            #worksheet.merge_range(row, 0,row,col_len,["Mesiac","Počet rozvozov","Hmotnosť rozvoz","Počet zvozov","Hmotnosť zvoz", "Σ (Rozvoz + zvoz)"],header_fmt)   
        row = row + len(dataframe.index) + spaces + 1

    bar["value"] = 55
    #maybe delete freezing to much
    worksheet.set_row(0,60)
    worksheet.set_column(0,0,10)
    worksheet.set_column(1,1,12)
    worksheet.set_column(2,2,13)
    worksheet.set_column(3,3,9)
    worksheet.set_column(4,4,12)
    worksheet.set_column(5,5,12)
    
    text_format = workbook.add_format({'text_wrap': True,"bold": True,
    'font_color': '#f1f1f1',"bg_color":"#303030","valign":"vcenter","align":"center"})
    
    for i in range (0,len(head_month)):
        worksheet.write(0,i,head_month[i],text_format)
    #############################DONE TO THIS##################################
    bar["value"] = 58
    leng = len(l_rozvoz)+ 1
    second_graph_posit = len(l_rozvoz)+ 24
    second_graph_posit = str(second_graph_posit)
    header_day = len(l_months) + 3
    for i in range (0,len(head_day)):
        worksheet.write(header_day,i,head_day[i],text_format)
    leng = str(leng)
    chart_st = 47
    chart1 = workbook.add_chart({"type": "line"})
    chart1.set_style(chart_st)
    chart1.add_series({
    "name": "rozvoz",
    'categories': '=Depo!$A$2:$A$'+leng,
    'values':     '=Depo!$B$2:$B$'+leng,
    })
    chart1.add_series({
    "name" : "zvoz",
    #'categories': '=Depo!$AS$2:$A$'+leng,
    'values':     '=Depo!$D$2:$D$'+leng,
    'line':   {'color': '#F95F3F'},
    })

    chart1.set_title({"name":"Počet rozvozov/zvozov"})
    chart1.set_x_axis({"name":"mesiac",'num_font':  {'rotation': -90}})
    chart1.set_y2_axis({"name":"zvozy"})
    chart1.set_y_axis({"name":"rozvozy"})
    chart1.set_style(chart_st)

    chart2 = workbook.add_chart({"type": "line"})
    chart2.set_style(chart_st)
    chart2.add_series({
    "name": "hmotnosť rozvoz",
    'categories': '=Depo!$A$2:$A$'+leng,
    'values':     '=Depo!$C$2:$C$'+leng,
    })

    chart2.add_series({
    "name" : "hmotnosť zvoz",
    #'categories': '=Depo!$AS$2:$A$'+leng,
    'values':     '=Depo!$E$2:$E$'+leng,
    'line':   {'color': '#F95F3F'},
    })

    chart2.set_title({"name":"Hmotnosť rozvozov/zvozov"})
    chart2.set_x_axis({"name":"mesiac",'num_font':  {'rotation': -90}})
    chart2.set_y2_axis({"name":"hm. zvozy"})
    chart2.set_y_axis({"name":"hm. rozvozy"})
    #chart1.set_plotarea({'fill':   {'color': '#303030'}})
    chart1.set_legend({'position': 'top'})
    chart2.set_legend({'position': 'top'})
    worksheet.insert_chart("H"+str(header_day),chart1,{'x_offset': 20, 
    'y_offset': 10,'x_scale': 1.4, 'y_scale': 1.2})
    worksheet.insert_chart("H"+second_graph_posit,chart2,{'x_offset': 20,
    'y_offset': 10,'x_scale': 1.4, 'y_scale': 1.2})
    return writer
    


def write_depo(depo_day,depo_month,list_rozvoz,list_zvoz,list_months):
    direct = os.getcwd()
    filename = direct + "/statistiky.xlsx"
    setup_depo = [depo_month,depo_day]
    bar["value"] = 48
    writer = multiple_dfs(setup_depo,"Depo",filename,2,5,
    list_rozvoz,list_zvoz,list_months)
    return writer


def write_emp(emp,writer):
    bar["value"] = 85
    root.update()
    direct = os.getcwd()
    filename = direct + "/statistiky.xlsx"
    row_len = len(emp) + 1
    graph_pos = row_len + 2
    row_len = str(row_len)
    
    emp.to_excel(writer, sheet_name='Zamestnanci', startcol=0,startrow=0,index=False)
    workbook  = writer.book  
    worksheet = writer.sheets["Zamestnanci"]
    header_emp = ["Meno vodiča","Mesiac výkonu","Počet stopov rozvoz",
    "Počet stopov zvoz","Hmotnosť obj. rozvoz","Hmotnosť obj. zvoz",
    "Počet doručených stopov","Počet zvezených stopov", 
    "Σ (Rozvoz + zvoz)","% úsp. rozvoz","% úst. zvoz"]
    
    text_format = workbook.add_format({'text_wrap': True,
    "bold": True, 'font_color': '#f1f1f1',"bg_color":"#303030",
    "valign":"vcenter","align":"center"})
    
    for i in range (0,len(header_emp)):
        worksheet.write(0,i,header_emp[i],text_format)
    
    worksheet.set_row(0,60)
    worksheet.set_column(0,0,22)
    worksheet.set_column(1,3,10)
    worksheet.set_column(4,4,18)
    worksheet.set_column(5,5,11)
    worksheet.set_column(6,7,12)
    worksheet.set_column(8,8,11)
    worksheet.set_column(9,10,10)
    worksheet.autofilter(0,0,0,10)
    chart1 = workbook.add_chart({"type": "column"})
    chart1.set_style(47)
    chart1.add_series({
    "name": "rozvoz",
    'categories': '=Zamestnanci!$A$2:$A$'+row_len,
    'values':     '=Zamestnanci!$C$2:$C$'+row_len,
    'data_labels': {'value': True},
    })
    chart1.add_series({
    "name" : "zvoz",
    #'categories': '=Depo!$AS$2:$A$'+leng,
    'values':     '=Zamestnanci!$D$2:$D$'+row_len,
    'data_labels': {'value': True},
    'fill':   {'color': '#F95F3F'}
    })
    chart1.set_y_axis({"name":"počet rozvozov/zvozov",'num_font':  {'rotation': 0}})
    chart1.set_legend({'position': 'top'})
    chart1.set_title({"name":'=Zamestnanci!$B$2',})
    worksheet.insert_chart("A"+str(graph_pos),chart1,{'x_offset': 15, 'y_offset': 10,
    'x_scale': 2.07, 'y_scale': 1.6})
    writer.save()



def parse_file(filename):
    bar["value"] = 10
    root.update()
    spec_columns = ["Meno vodiča", "Dátum výkonu", "počet doručených stopov",
    "hmotnosť doručených objednávok", "počet zvezených stopov", "hmotnosť zvezených objednávok",
    "počet stopov rozvoz", "počet stopov zvoz"]
    excel_data_df = pandas.read_excel(filename,usecols=spec_columns,dtype={'Meno vodiča': str,
    'počet doručených stopov': int,'hmotnosť doručených objednávok': float,
    'počet zvezených stopov': int,'hmotnosť zvezených objednávok': float,
    'počet stopov rozvoz': int,'počet stopov zvoz': float, })
    
    # print whole sheet data
    names = []
    months = []
    years = []
    days = []
    # store names in list
    for i in excel_data_df.index:
        if excel_data_df["Meno vodiča"][i] not in names:
            names.append(excel_data_df["Meno vodiča"][i])
    # change date format
    for i in excel_data_df.index:

        if pandas.isna(excel_data_df["hmotnosť doručených objednávok"][i]):
            excel_data_df["hmotnosť doručených objednávok"][i] = 0.0
        
        if pandas.isna(excel_data_df["hmotnosť zvezených objednávok"][i]):
            excel_data_df["hmotnosť zvezených objednávok"][i] = 0.0
        
        split = re.split("\.",excel_data_df["Dátum výkonu"][i])
        newval = split[2]+"-"+split[1]+"-"+split[0]
        if newval not in days:
            days.append(newval)
        year = split[2]
        if year not in years:
            years.append(year)
        
        month = split[2]+"-"+split[1]
        if month not in months:
            months.append(month)

        excel_data_df["Dátum výkonu"][i] = excel_data_df["Dátum výkonu"][i].replace(excel_data_df["Dátum výkonu"][i],newval)
    bar["value"] = 20
    root.update()
    writer = parse_depo(excel_data_df,days,months,years)
    bar["value"] = 65
    root.update()
    parse_people(excel_data_df,names,months,years,writer)
    os.system("start excel.exe statistiky.xlsx")
    sys.exit(0)


    
def open_excel_file():
    filetypes = (('Excel súbory', '*.xlsx'),('Všetky súbory', '*.*'))
    f = fd.askopenfile(filetypes=filetypes)
    parse_file(f.name)


root = tk.Tk()
root.configure(background='#303030')
root.title('Nahrať štatistiky')
root.geometry('720x500')

progress_bar = ttk.Progressbar(root, orient="horizontal",mode="determinate", maximum=100, value=0)
progress_bar.start()
progress_bar.step(5)
progress_bar["value"] = 0

root.update()
style = Style()
style.configure('W.TButton', font =('calibri', 11,),foreground = '#303030')

open_button = ttk.Button(root,text='Vybrať súbor',style = 'W.TButton',command=open_excel_file)
open_button.grid(column=1, row=1, sticky='',padx=300,pady=100)

bar = Progressbar(root, length=200, style='black.Horizontal.TProgressbar')
bar.grid(column=1, row=2)
bar["value"] = 0

label = ttk.Label(root,text = "© Valaštín 2021",background="#303030",foreground="#f1f1f1")
label.grid(column = 1, row = 3,pady= 20)

root.mainloop()
