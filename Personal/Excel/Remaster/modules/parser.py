import os
import re
import pandas
import xlsxwriter
import tkinter as tk
from tkinter import ttk
from tkinter.ttk import Progressbar,Style
from datetime import datetime
from modules.values import Values
from modules.frames import Frame
from modules.depo import Depo
from modules.empl import Empl
from modules.stats import Stats

class Parser():
    def __init__(self, filename,window):
        self.rv = 0
        self.window = window
        self.init_bar()
        self.parse_file(filename)
        
    def init_bar(self):
        self.window.bar['value'] = 0
    
    def inc_bar(self):
        self.window.bar['value'] = self.window.bar['value'] + 12.5
        
    def parse_file(self,filename):
        self.inc_bar()
        self.window.root.update()
        
        if filename is None:
            self.rv = Values.INVALID_FILE
            return
        
        if not re.match('^.+(xlsx|xls)$', filename.name):
            self.rv = Values.INVALID_FILE
            return 
           
        spec_columns = ['Meno vodiča', 'Dátum výkonu', 'počet doručených stopov',
        'hmotnosť doručených objednávok', 'počet zvezených stopov', 
        'hmotnosť zvezených objednávok', 'počet stopov rozvoz', 'počet stopov zvoz']
        
        excel_data_df = pandas.read_excel(open(filename.name,'rb'), usecols=spec_columns,
        dtype = {'Meno vodiča': str, 'počet doručených stopov': int,
        'hmotnosť doručených objednávok': float, 'počet zvezených stopov': int,
        'hmotnosť zvezených objednávok': float, 'počet stopov rozvoz': int,
        'počet stopov zvoz': float})
        
        lists = Frame()
        
        stats = Stats(excel_data_df)
        
        date = ttk.Label(self.window.root, 
        text = 'Analyzované obdobie: ' + str(stats.first_date) + ' - ' + str(stats.last_date), 
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        date.grid(column = 1, row = 4, pady = 10, padx=20)
        
        count = ttk.Label(self.window.root, 
        text = 'Celkový počet výkonov: ' + str(stats.count), 
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        count.grid(column = 1, row = 5, pady = 10, padx=20)
        
        
        month_stats = ttk.Label(self.window.root, 
        text = 'PRIEMERNÉ MESAČNÉ ŠTATISTIKY', 
        background='#303030', foreground='#f1f1f1', anchor='w', font='Helvetica 13 bold')
        month_stats.grid(column = 0, row = 6, pady = 10, padx=20)
        
        full_stats = ttk.Label(self.window.root, 
        text = 'CELKOVÉ ŠTATISTIKY', 
        background='#303030', foreground='#f1f1f1', anchor='w', font='Helvetica 13 bold')
        full_stats.grid(column = 2, row = 6, pady = 10, padx=20)
        
        for record in excel_data_df.index:
            # update invalid floats
            if pandas.isna(excel_data_df['hmotnosť doručených objednávok'][record]):
                excel_data_df.loc[record,'hmotnosť doručených objednávok'] = 0.0
            if pandas.isna(excel_data_df['hmotnosť zvezených objednávok'][record]):
                excel_data_df.loc[record,'hmotnosť zvezených objednávok'] = 0.0
            
            #append drivers name to list
            if not lists.in_names(excel_data_df['Meno vodiča'][record]):
                lists.add_name(excel_data_df['Meno vodiča'][record])
            
            #update date format - split_date[0] - day, split_date[1] - month, split_date[2] - year
            split_date = re.split('\.', excel_data_df['Dátum výkonu'][record])
            new_format = split_date[2] + '-' + split_date[1] + '-' + split_date[0]
            
            if not lists.in_days(new_format):
                lists.add_day(new_format)
            if not lists.in_years(split_date[2]):
                lists.add_year(split_date[2])
            month_format = split_date[2] + '-' + split_date[1]
            if not lists.in_months(month_format):
                lists.add_month(month_format)
            
            #update date format in 'Dátum výkonu' column
            excel_data_df.loc[record,'Dátum výkonu'] = new_format
                
        self.inc_bar()
        self.window.root.update()
        self.parse_data(excel_data_df, lists, stats)
        
    def display_stats(self, stats):
        average_rozvoz = ttk.Label(self.window.root, 
        text = 'Priemerný mesačný rozvoz: ' + str(stats.average_month_rozvoz),
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        average_rozvoz.grid(column = 0, row = 7, pady = 10, padx=20)
        
        average_zvoz = ttk.Label(self.window.root, 
        text = 'Priemerný mesačný zvoz: ' + str(stats.average_month_zvoz),
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        average_zvoz.grid(column = 0, row = 8, pady = 10, padx=10)
        
        average_rozvoz_kgs = ttk.Label(self.window.root, 
        text = 'Priemerná hmotnosť rozvoz: ' + str(stats.average_month_rozvoz_kgs)  + ' kg',
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        average_rozvoz_kgs.grid(column = 0, row = 9, pady = 10, padx=20)
        
        average_zvoz_kgs = ttk.Label(self.window.root, 
        text = 'Priemerná hmotnosť zvoz: ' + str(stats.average_month_zvoz_kgs) + ' kg',
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        average_zvoz_kgs.grid(column = 0, row = 10, pady = 10, padx=20)
        
        sum_rozvoz = ttk.Label(self.window.root, 
        text = 'Celkový rozvoz: ' + str(stats.pocet_rozvozov),
        background='#303030', foreground='#f1f1f1', font = ('Arial' , 11))
        sum_rozvoz.place(anchor='w')
        sum_rozvoz.grid(column = 2, row = 7, pady = 10, padx=0)
        
        sum_zvoz = ttk.Label(self.window.root, 
        text = 'Celkový zvoz: ' + str(stats.pocet_zvozov), 
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        sum_zvoz.grid(column = 2, row = 8, pady = 10, padx=0)
        
        sum_rozvoz_kgs = ttk.Label(self.window.root, 
        text = 'Celková hmotnosť rozvoz: ' + str(stats.hmotnost_rozvoz) + ' kg',
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        sum_rozvoz_kgs.grid(column = 2, row = 9, pady = 10, padx=0)
        
        sum_zvoz_kgs = ttk.Label(self.window.root, 
        text = 'Celková hmotnosť zvoz: ' + str(stats.hmotnost_zvoz) + ' kg',
        background='#303030', foreground='#f1f1f1', anchor='w', font = ('Arial' , 11))
        sum_zvoz_kgs.grid(column = 2, row = 10, pady = 10, padx=0)
        self.window.root.update()
    
        
    def parse_data(self, df, lists, stats):
        depo = Depo()
        depo.parse_depo_daily(df, lists) 
        self.inc_bar()
        self.window.root.update()
        depo.parse_depo_monthly(df, lists)
        stats.calculate_average(depo.month_dataframe)
        self.display_stats(stats)
        self.inc_bar()
        self.window.root.update()
        empl = Empl()
        empl.parse_empl(df, lists)
        self.inc_bar()
        self.window.root.update()
        self.create_sheet(depo,empl)
     
    def crate_depo_charts(self, workbook, worksheet, months_len, header_day_position, second_g_position):
        chart_style = 47
        chart1 = workbook.add_chart({'type': 'line'})
        chart1.set_style(chart_style)
        chart1.add_series({
        'name': 'rozvoz',
        'categories': '=Depo!$A$2:$A$'+ months_len,
        'values':     '=Depo!$B$2:$B$'+ months_len,
        })
        chart1.add_series({
        'name' : 'zvoz',
        'values':     '=Depo!$D$2:$D$'+ months_len,
        'line':   {'color': '#F95F3F'},
        })

        chart1.set_title({'name': 'Počet rozvozov/zvozov'})
        chart1.set_x_axis({'name': 'mesiac','num_font':  {'rotation': -90}})
        chart1.set_y2_axis({'name': 'zvozy'})
        chart1.set_y_axis({'name': 'rozvozy'})

        chart2 = workbook.add_chart({'type': 'line'})
        chart2.set_style(chart_style)
        chart2.add_series({
        'name': 'hmotnosť rozvoz',
        'categories': '=Depo!$A$2:$A$'+ months_len,
        'values':     '=Depo!$C$2:$C$'+ months_len,
        })

        chart2.add_series({
        'name' : 'hmotnosť zvoz',
        'values':     '=Depo!$E$2:$E$'+ months_len,
        'line':   {'color': '#F95F3F'},
        })

        chart2.set_title({'name': 'Hmotnosť rozvozov/zvozov'})
        chart2.set_x_axis({'name': 'mesiac','num_font':  {'rotation': -90}})
        chart2.set_y2_axis({'name': 'hm. zvozy'})
        chart2.set_y_axis({'name': 'hm. rozvozy'})
        chart1.set_legend({'position': 'top'})
        chart2.set_legend({'position': 'top'})
        worksheet.insert_chart('H' + str(header_day_position), chart1, {'x_offset': 20, 
        'y_offset': 10, 'x_scale': 1.4, 'y_scale': 1.2})
        worksheet.insert_chart('H' + str(second_g_position),chart2,{'x_offset': 20,
        'y_offset': 10,'x_scale': 1.4, 'y_scale': 1.2})
         
    
    def setup_worksheet_positioning(self, worksheet):
        worksheet.set_row(0,60)
        worksheet.set_column(0,0,10)
        worksheet.set_column(1,1,12)
        worksheet.set_column(2,2,13)
        worksheet.set_column(3,3,9)
        worksheet.set_column(4,4,12)
        worksheet.set_column(5,5,12)
        
    def setup_empl_worksheet(self,worksheet):
        worksheet.set_row(0,60)
        worksheet.set_column(0,0,22)
        worksheet.set_column(1,3,10)
        worksheet.set_column(4,4,18)
        worksheet.set_column(5,5,11)
        worksheet.set_column(6,7,12)
        worksheet.set_column(8,8,11)
        worksheet.set_column(9,10,10)
        worksheet.autofilter(0,0,0,10)
        
    def setup_empl_graph(self, workbook, worksheet, empl_row_len, empl_graph_pos):
        chart1 = workbook.add_chart({'type': 'column'})
        chart1.set_style(47)
        chart1.add_series({
        'name': 'rozvoz',
        'categories': '=Zamestnanci!$A$2:$A$'+ empl_row_len,
        'values':     '=Zamestnanci!$C$2:$C$'+ empl_row_len,
        'data_labels': {'value': True},
        })
        chart1.add_series({
        'name' : 'zvoz',
        'values':     '=Zamestnanci!$D$2:$D$'+ empl_row_len,
        'data_labels': {'value': True},
        'fill':   {'color': '#F95F3F'}
        })
        chart1.set_y_axis({'name': 'počet rozvozov/zvozov','num_font':  {'rotation': 0}})
        chart1.set_legend({'position': 'top'})
        chart1.set_title({'name': 'Zamestnanci',})
        worksheet.insert_chart('A' + str(empl_graph_pos), chart1,{'x_offset': 15, 'y_offset': 10,
        'x_scale': 2.07, 'y_scale': 1.6})
        
            
    def create_sheet(self, depo, empl):
        curr_direc = os.getcwd()
        date = datetime.now().strftime('%Y_%m_%d_%H-%M-%S')
        filename = curr_direc + '\SDS_TN-' + date + '.xlsx'
        
        header_depo_day = ['Deň', 'Počet rozvozov', 'Hmotnosť rozvoz',
        'Počet zvozov', 'Hmotnosť zvoz', 'Σ (Rozvoz + zvoz)']
        header_depo_month = ['Mesiac', 'Počet rozvozov', 'Hmotnosť rozvoz',
        'Počet zvozov', 'Hmotnosť zvoz', 'Σ (Rozvoz + zvoz)']
        with pandas.ExcelWriter(filename,engine='xlsxwriter') as writer:
            workbook  = writer.book   
            header_fmt = workbook.add_format({'font_name': 'Arial', 'font_size': 10,
            'bold': True, 'bg_color': '#303030'})
            depo_data = [depo.month_dataframe, depo.day_dataframe]
            row = 0
            for dataframe in depo_data:
                dataframe.to_excel(writer, 'Depo', startrow=row, startcol=0, index=False)
                if row == 0:
                    worksheet = writer.sheets['Depo']
                    worksheet.autofilter(row,0,row,5)
                row += len(dataframe.index) + 2 + 1
            
            #design
            self.setup_worksheet_positioning(worksheet)
            text_format = workbook.add_format({'text_wrap': True, 'bold': True,
            'font_color': '#f1f1f1', 'bg_color': '#303030', 'valign':'vcenter', 'align': 'center'})
            
            self.inc_bar()
            self.window.root.update()
            for i in range (0, len(header_depo_month)):
                worksheet.write(0, i, header_depo_month[i], text_format)
            
            months_len = len(depo.kgs_rozvoz) + 1
            second_g_position = months_len + 24
            months_len = str(months_len)
            header_day_position = len(depo.kgs_mnths) + 3
            
            for i in range(0, len(header_depo_day)):
                worksheet.write(header_day_position,i,header_depo_day[i],text_format)
                
            self.inc_bar()
            self.window.root.update()
            self.crate_depo_charts(workbook, worksheet, months_len, header_day_position, second_g_position)
            
            #create second worksheet for emp performance
            header_emp = ['Meno vodiča', 'Mesiac výkonu', 'Počet stopov rozvoz',
            'Počet stopov zvoz', 'Hmotnosť obj. rozvoz', 'Hmotnosť obj. zvoz',
            'Počet doručených stopov', 'Počet zvezených stopov', 
            'Σ (Rozvoz + zvoz)', '% úsp. rozvoz', '% úst. zvoz']
            
            empl.emp_result.to_excel(writer, sheet_name='Zamestnanci', startcol=0,startrow=0,index=False)
            workbook  = writer.book  
            worksheet = writer.sheets['Zamestnanci']
            for i in range (0,len(header_emp)):
                worksheet.write(0,i,header_emp[i],text_format)
            
            self.setup_empl_worksheet(worksheet)
            
            empl_row_len = len(empl.emp_result) + 1
            empl_graph_pos = empl_row_len + 2
            empl_row_len = str(empl_row_len)
            self.setup_empl_graph(workbook, worksheet, empl_row_len, empl_graph_pos)
            self.inc_bar()
            self.window.root.update()
            open_string = 'start excel.exe ' + filename
            os.system(open_string)
            
            