import re
import pandas

class Empl():
    def __init__(self):
        self.emp_names = []
        self.emp_months = []
        self.emp_performance = []
        self.emp_result = None
        
        #employee attr
        self.name = ''
        self.month = ''
        self.pocet_stopov_rozvoz = 0
        self.pocet_stopov_zvoz = 0
        self.hmotnost_dor_zas = 0.0
        self.hmotnost_zvez_zas = 0.0
        self.pocet_dor_stopov = 0
        self.pocet_zvez_stopov = 0
        self.uspesnost_rozvoz = 0
        self.uspesnost_zvoz = 0
        self.rozvoz_zvoz = 0
             
    
    def parse_empl(self, df, lists):
        for emp_name in lists.names:
            name_split = df[df['Meno vodiča'].str.match(emp_name) == True]
            self.add_emp_name(name_split)
        
        for month in lists.months:
            for empl in self.emp_names:
                split = empl[empl['Dátum výkonu'].str.match(month) == True]
                if len(split):
                    self.add_month_emp(split)
                    
        for this_emp_in_this_month in self.emp_months:
            for record in this_emp_in_this_month.index:
                self.name = this_emp_in_this_month['Meno vodiča'][record]
                self.month = this_emp_in_this_month['Dátum výkonu'][record]
                self.pocet_stopov_rozvoz += this_emp_in_this_month['počet stopov rozvoz'][record]
                self.pocet_stopov_zvoz += this_emp_in_this_month['počet stopov zvoz'][record]
                self.hmotnost_dor_zas += this_emp_in_this_month['hmotnosť doručených objednávok'][record]
                self.hmotnost_zvez_zas += this_emp_in_this_month['hmotnosť zvezených objednávok'][record]
                self.pocet_dor_stopov += this_emp_in_this_month['počet doručených stopov'][record]
                self.pocet_zvez_stopov += this_emp_in_this_month['počet zvezených stopov'][record]
            
            self.calculate_suc()
            self.calculate_sum()
            month_split = re.split('\-',self.month)
            self.month = month_split[0] + '-' + month_split[1]
            # store performance
            performance = [self.name, self.month, self.pocet_stopov_rozvoz,
            self.pocet_stopov_zvoz, round(self.hmotnost_dor_zas, 2), round(self.hmotnost_zvez_zas, 2),
            self.pocet_dor_stopov, self.pocet_zvez_stopov, self.rozvoz_zvoz,
            round(self.uspesnost_rozvoz, 2), round(self.uspesnost_zvoz, 2)]
            self.add_performance(performance)
            self.reset_attr()
            
        self.emp_result = pandas.DataFrame(self.emp_performance, columns = [
        'Meno vodiča', 'Mesiac výkonu', 'Počet stopov rozvoz',
        'Počet stopov zvoz', 'Hmotnosť obj. rozvoz', 'Hmotnosť obj. zvoz',
        'Počet doručených stopov', 'Počet zvezených stopov', 
        'Σ (Rozvoz + zvoz)', '% úsp. rozvoz', '% úst. zvoz'])

    
    def reset_attr(self):
        self.name = ''
        self.month = ''
        self.pocet_stopov_rozvoz = 0
        self.pocet_stopov_zvoz = 0
        self.hmotnost_dor_zas = 0.0
        self.hmotnost_zvez_zas = 0.0
        self.pocet_dor_stopov = 0
        self.pocet_zvez_stopov = 0
        self.uspesnost_rozvoz = 0
        self.uspesnost_zvoz = 0
        self.rozvoz_zvoz = 0
        
    def add_performance(self,perf):
        self.emp_performance.append(perf)
    
    def calculate_sum(self):
        self.rozvoz_zvoz = self.pocet_stopov_rozvoz + self.pocet_stopov_zvoz
    
    def calculate_suc(self):
        if self.pocet_dor_stopov > 0 and self.pocet_stopov_rozvoz > 0:
            self.uspesnost_rozvoz = ((self.pocet_dor_stopov / self.pocet_stopov_rozvoz) * 100)
        if self.pocet_zvez_stopov > 0 and self.pocet_stopov_zvoz > 0:
            self.uspesnost_zvoz = ((self.pocet_zvez_stopov / self.pocet_stopov_zvoz) * 100)
    
    def add_month_emp(self,split):
        self.emp_months.append(split)
    
    def add_emp_name(self,name):
        self.emp_names.append(name)