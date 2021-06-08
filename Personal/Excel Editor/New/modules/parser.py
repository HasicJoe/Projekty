import os
import sys
import re
import pandas
from modules.values import Values
from modules.frames import Frame

class Parser():
    def __init__(self, filename,window):
        self.bar_value = 0
        self.rv = 0
        self.window = window
        self.parse_file(filename)
    
    def inc_bar(self):
        self.window.bar['value'] = self.window.bar['value'] + 10
        
    def parse_file(self,filename):
        self.inc_bar()
        
        if not re.match("^.+(xlsx|xls)$", filename.name):
            print(Values.INVALID_FILE)
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
            