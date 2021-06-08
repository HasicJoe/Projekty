import pandas
import re

class Depo():
    def __init__(self):
        
        # day values
        self.hmotnost_rozvozu = 0.0
        self.hmotnost_zvozu = 0.0
        self.pocet_zvozov = 0
        self.pocet_rozvozov = 0
        self.day_list = []
        self.day_dataframe = None
        
        # month values
        self.hmotnost_rozvozu_mon = 0.0
        self.hmotnost_zvozu_mon = 0.0
        self.pocet_zvozov_mon = 0
        self.pocet_rozvozov_mon = 0
        self.kgs_rozvoz = []
        self.kgs_zvoz = []
        self.kgs_mnths = []
        self.months = []
        self.month_list = []
        self.month_dataframe = None
    
    def add_rozvoz_month_kgs(self,value):
        self.kgs_rozvoz.append(value)
    
    def add_zvoz_month_kgs(self,value):
        self.kgs_zvoz.append(value)
    
    def add_kgs_mnths(self,value):
        self.kgs_mnths.append(value)
    
    def add_months(self,value):
        self.months.append(value)
    
    def add_monthly_perf(self,value):
        self.month_list.append(value)
        
    def parse_depo_monthly(self, df, lists):
        for this_month in lists.months:
            month_split = df[df['Dátum výkonu'].str.match(this_month) == True]
            self.add_month(month_split)
        
        for this_mon in self.months:
            for record in this_mon.index:
                month_format = this_mon.loc[record]['Dátum výkonu']
                self.hmotnost_rozvozu_mon += this_mon.loc[record]['hmotnosť doručených objednávok']
                self.hmotnost_zvozu_mon += this_mon.loc[record]['hmotnosť zvezených objednávok']
                self.pocet_rozvozov_mon += this_mon.loc[record]['počet doručených stopov']
                self.pocet_zvozov_mon += this_mon.loc[record] ['počet zvezených stopov']
            
            sum = self.pocet_rozvozov_mon + self.pocet_zvozov_mon
            month_split = re.split('\-', month_format)
            month_fix_value = month_split[0] + '-' + month_split[1]
            self.add_rozvoz_month_kgs(self.hmotnost_rozvozu_mon)
            self.add_zvoz_month_kgs(self.hmotnost_zvozu_mon)
            self.add_kgs_mnths(month_fix_value)
            
            month_performance = [month_fix_value, self.pocet_rozvozov_mon,
            round(self.hmotnost_rozvozu_mon,2), self.pocet_zvozov_mon,
            round(self.hmotnost_zvozu_mon, 2), sum]
            
            self.add_monthly_perf(month_performance)
            self.reset_values_monthly()
        
        self.month_dataframe = pandas.DataFrame(self.month_list, columns = ['Mesiac', 
        'Počet rozvozov', 'Hmotnosť rozvoz', 'Počet zvozov', 'Hmotnosť zvoz', 'Σ (Rozvoz + zvoz)'])
               
        #print(self.day_list)  
        #print("________________________________________________________")      
        #print(self.month_list)
     
                    
    def reset_values_monthly(self):
        self.hmotnost_rozvozu_mon = 0.0
        self.hmotnost_zvozu_mon = 0.0
        self.pocet_rozvozov_mon = 0
        self.pocet_zvozov_mon = 0
        
            
    def parse_depo_daily(self, df, lists):
        
        for this_day in lists.days:
            for record in df.index:
                if df['Dátum výkonu'][record] == this_day:
                    self.hmotnost_rozvozu += df['hmotnosť doručených objednávok'][record]
                    self.hmotnost_zvozu += df['hmotnosť zvezených objednávok'][record]
                    self.pocet_rozvozov += df['počet zvezených stopov'][record]
                    self.pocet_zvozov += df['počet doručených stopov'][record]
            sum = self.pocet_rozvozov + self.pocet_zvozov
            day_performance = [this_day, self.pocet_rozvozov, self.hmotnost_rozvozu,
                         self.pocet_zvozov, self.hmotnost_zvozu, sum]
            self.add_performance(day_performance)
            self.reset_values_daily()
        self.day_dataframe = pandas.DataFrame(self.day_list, columns = ['Deň',
        'Počet rozvozov', 'Hmotnosť rozvoz', 'Počet zvozov',
        'Hmotnosť zvoz', 'Σ (Rozvoz + zvoz)'])
                
    def add_month(self,this_mon):
        self.months.append(this_mon)
        
    def add_performance(self, performance):
        self.day_list.append(performance)
    
    def reset_values_daily(self):
        self.hmotnost_rozvozu = 0.0
        self.hmotnost_zvozu = 0.0
        self.pocet_rozvozov = 0
        self.pocet_zvozov = 0
        
        