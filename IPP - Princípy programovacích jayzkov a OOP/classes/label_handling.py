class Label:


    def __init__(self,order,label_name):
        self.order = order
        self.label_name = label_name
        
        
    def __str__(self):
        return "ORDER: %s, LABEL NAME: %s" % (self.order,self.label_name)
