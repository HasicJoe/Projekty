class Stats():
    def __init__(self,df):
        self.first_date = df['Dátum výkonu'].iloc[0]
        self.last_date = df['Dátum výkonu'].iloc[-1]
        self.count = len(df)
        self.pocet_rozvozov = 0
        self.pocet_zvozov = 0
        self.hmotnost_rozvoz = 0.0
        self.hmotnost_zvoz = 0.0
        self.average_month_rozvoz = 0
        self.average_month_zvoz = 0
        self.average_month_rozvoz_kgs = 0.0
        self.average_month_zvoz_kgs = 0.0
    
    def calculate_average(self,depo_df):
        self.pocet_rozvozov = depo_df['Počet rozvozov'].sum()
        self.pocet_zvozov = depo_df['Počet zvozov'].sum()
        self.hmotnost_rozvoz = round(depo_df['Hmotnosť rozvoz'].sum(),2)
        self.hmotnost_zvoz = round(depo_df['Hmotnosť zvoz'].sum(),2)
        self.average_month_rozvoz = round((self.pocet_rozvozov / len(depo_df)),2)
        self.average_month_zvoz = round((self.pocet_zvozov / len(depo_df)),2)
        self.average_month_rozvoz_kgs = round((self.hmotnost_rozvoz / len(depo_df)),2)
        self.average_month_zvoz_kgs = round((self.hmotnost_zvoz / len(depo_df)),2)
        