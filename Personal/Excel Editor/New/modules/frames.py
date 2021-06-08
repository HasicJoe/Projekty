
class Frame:
    def __init__(self):
        self.names = []
        self.months = []
        self.years = []
        self.days = []
    
    def add_name(self, name):
        self.names.append(name)
    
    
    def in_names(self,this_name):
        for name in self.names:
            if name == this_name:
                return True        
        return False
    
    def add_month(self, month):
        self.months.append(month)
        
        
    def in_months(self,this_month):
        for month in self.months:
            if month == this_month:
                return True
        return False
    
    
    def add_year(self, year):
        self.years.append(year)
       
        
    def in_years(self,this_year):
        for year in self.years:
            if year == this_year:
                return True
        return False
    
    
    def add_day(self, day):
        self.days.append(day)
        
    def in_days(self,this_day):
        for day in self.days:
            if day == this_day:
                return True
        return False
    
        
    def __repr__(self):
        return str(self)