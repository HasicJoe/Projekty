
"""@brief code for calculating standard deviation 
   
   @author IVS-DREAM-TEAM
   
   @file profiler.py
"""




import re
import lib.mathlib as ml
import cProfile

class Profiler():
    def __init__(self,data,pr):
        self.numbers = []
        self.sum = 0
        self.arith_avg = 0
        self.in_br = 0
        self.before_br = 0
        self.std_dev = 0
        self.parse_numbers(data,pr)
        
    def parse_numbers(self,data,pr):
        parsed_data = []
        for line in data:
            parsed_data.append(re.split("\s+",line))
        for line in parsed_data:
            for element in line:
                if self.is_int(element) or self.is_float(element):
                    self.numbers.append(float(element))
        self.start_profiling(pr)
    
    
    def is_int(self,element):
        try:
            int(element)
            return True
        except:
            return False
        
    
    def is_float(self,element):
        try:
            float(element)
            return True
        except:
            return False
    
    def start_profiling(self,pr):
        pr.enable()
        # Calc sum of all numbers
        for i in range(0,len(self.numbers) - 1,2):
            self.sum += ml.add(self.numbers[i],self.numbers[i+1])
        
        # in case of odd len of numbers    
        if ml.mod(len(self.numbers),2) == 1:
            self.sum += self.numbers[len(self.numbers)-1]
        
        # Calc arithmetic. average
        self.arith_avg = ml.arith_average(self.sum,self.numbers)
        
        for i in self.numbers:
            self.in_br += ml.exp(i,2)
        
        self.in_br -= ml.list_len(self.numbers) * ml.exp(self.arith_avg,2)
        self.before_br = ml.div(1,ml.list_len(self.numbers)-1)
        self.std_dev = ml.root(2,ml.mul(self.before_br,self.in_br))
        pr.disable()
        pr.print_stats()
        print("Smerodatná odchylka: ",self.std_dev)
        print("Počet vzoriek:",len(self.numbers))
        print("Aritmetický priemer: ",self.arith_avg)