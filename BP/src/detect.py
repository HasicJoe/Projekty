"""
Autor: Samuel Valaštín
Dátum : 28.1.2022
Súbor metód pre detekciu a načítanie logovacieho formátu a logovacích dát.
    = podporuje spracovanie komprimovanych logovacich suborov
        - gzip = 1 subor
        - zip = viac suborov a zahrna spojenie viacerych logovacich suborov
    = odfiltrovanie nepotrebnych dat
    = transformaciu niektorych poli pre dane logovacie formaty

"""


from flask import Blueprint
import gzip, zipfile, io, re, os
import mimetypes
import apachelogs
import pandas as pd


detect = Blueprint("detect", __name__)


class LogHeaders:
    COMBINED = [
        "Remote Host",
        "Client Identifier",
        "User Name",
        "Date and Time",
        "Timezone",
        "Request",
        "State Code",
        "Transfer Volume",
        "Referer",
        "User Agent",
    ]
    COMMON = [
        "Remote Host",
        "Client Identifier",
        "User Name",
        "Date and Time",
        "Timezone",
        "Request",
        "State Code",
        "Transfer Volume",
    ]
    QUERY = ["ID", "Query", "QueryTime", "ItemRank", "ClickURL"]


class SupportedHeaders:
    COMMON = 1
    COMBINED = 2 
    QUERY = 3


class FormatAnalyzer:
    """
    Trieda pre detekciu standardnych logovacich formatov a nasledne spracovanie obsahu do dataframu.
    """
    def __init__(self, log_file):
        mimetypes.add_type('text/x-log', ".log")
        self.formats = list()
        self.readme = False
        self.df = None
        self.error = False
        self.detected = self.detect_format(log_file)
        self.make_memory_safe()


    def make_memory_safe(self):
        """
        Zbavenie sa nepotrebnych poli + konverzia na casovy typ datetime
        """
        try:
            if self.format == SupportedHeaders.COMBINED or self.format == SupportedHeaders.COMMON:
                if self.format == SupportedHeaders.COMBINED:
                    columns = LogHeaders.COMBINED[0:1] + LogHeaders.COMBINED[3::]
                elif self.format == SupportedHeaders.COMMON:
                    columns = LogHeaders.COMMON[0:1] + LogHeaders.COMMON[3::]
                """
                odfiltrovanie zaznamov s chybajucimi informaciami, je potrebne
                tuto cinnost vykonat pred dalsim predspracovanim
                """
                for col in columns:
                    self.df = self.df[(~self.df[col].isnull())]
                
                """
                zbavenie sa sumu pred dalsim predspracovanim
                """
                self.df = self.df[(self.df['Transfer Volume'] != '-') & (self.df['State Code'] != '-')]            
                self.df = self.df.drop(['Client Identifier', 'User Name', 'Timezone'], axis=1)
                self.df['State Code'] = self.df['State Code'].astype('uint16')
                self.df['Transfer Volume'] = self.df['Transfer Volume'].astype('uint32')
                """
                zbavenie sa [] z pola urcujuceho datum a cas a nasledna konverzia
                """
                self.df['Date and Time'] = self.df['Date and Time'].str[1::]
                self.df['Date and Time'] = pd.to_datetime(self.df['Date and Time'], dayfirst=True, format="%d/%b/%Y:%H:%M:%S")
            elif self.format == SupportedHeaders.QUERY:
                """
                Ponechane pre buduce rozsirenie aplikacie
                """
                self.error = True
        except:
            self.error = True
            

    def create_dataframe(self, log_file, compression = 'infer', multiple_files = False, filenames = None):
        """
        Vytvorenie pandas dataframu -> v pripade neznameho formatu vrati metoda False.
        """
        try:
            # moznost optimalizacie
            if self.format == SupportedHeaders.COMMON:
                return pd.read_csv(log_file, sep=" ", quotechar='"', compression=compression, on_bad_lines='skip', header=None, names=LogHeaders.COMMON)
            elif self.format == SupportedHeaders.COMBINED:
                return pd.read_csv(log_file, sep=" ", quotechar='"', compression=compression, on_bad_lines='skip', header=None, names=LogHeaders.COMBINED)
            elif self.format == SupportedHeaders.QUERY:
                return pd.read_csv(log_file, sep="\t", header=0, names=LogHeaders.QUERY, compression=compression, on_bad_lines='skip')
        except Exception as e:
            return False 
       
       
    def detect_format(self, log_file):
        """
        Otvori, rozbali subory s logovacimi zaznamami a zavola obsluznu funkciu pre detekciu formatov a nasledne poziada o vytvorenie dataframu.
        """
        if log_file.content_type:
            if log_file.content_type == 'text/plain' or log_file.content_type == 'text/x-log':
                try:
                    """
                    detekcia nekomprimovaneho suboru
                    """
                    f = io.TextIOWrapper(log_file)
                    self.format = self.analyze_entry(f)
                    self.df = self.create_dataframe(log_file)
                    return True
                except:
                    return False
            elif log_file.content_type == 'application/gzip':
                """
                detekcia komprimovaneho GZIP archivu
                """
                try:
                    with gzip.open(log_file, 'r') as gzip_f:
                        with io.TextIOWrapper(gzip_f, encoding='utf-8') as f:
                            self.format = self.analyze_entry(f)
                            self.df = self.create_dataframe(log_file, compression='gzip')        
                except:
                    return False
            elif log_file.content_type == 'application/zip' and zipfile.is_zipfile(log_file):
                """
                detekcia komprimovaneho ZIP archivu
                    umoznuje spojenie viacerych logovacich suborov vo vnutri archivu
                """
                try:
                    with zipfile.ZipFile(log_file, mode='r', allowZip64=True) as zip_f:
                        files = [file for file in zip_f.namelist() if not os.path.isdir(file)]
                        for file in files:
                            file_mime = mimetypes.guess_type(file)[0]
                            if file_mime == 'text/plain' or file_mime == 'text/x-log':
                                with zip_f.open(file, 'r') as file_in_zip:
                                    f = io.TextIOWrapper(file_in_zip)    
                                    if self.add_format_to_formats(self.analyze_entry(f)):
                                        if self.readme:
                                            self.readme = False
                                        else:
                                            if self.df is None:
                                                self.df = self.create_dataframe(file_in_zip)
                                            else:
                                                tmp_df = self.create_dataframe(file_in_zip)
                                                self.df = pd.concat([self.df, tmp_df])
                        """
                        zbavenie sa indexov pre ich prekrivanie po konkatenacii.
                        """
                        self.df = self.df.reset_index(drop=True)
                except Exception:
                    return False
            return True
        

    def add_format_to_formats(self, new_f):
        """
        Pridanie formátu záznamu súborov komprimovaných zip.
        """ 
        if new_f is None:
            self.readme = True
            return True
        elif len(self.formats) == 0 and new_f is not None:
            self.format = new_f
            self.formats.append(new_f)
            return True
        elif len(self.formats) == 1 and new_f == self.formats[0]:
            return True
        return False
        

    def analyze_entry(self, log_file, entry = None):
        """
        Analyza zaznamu logovacieho suboru -> podla zaznamu sa urci prislusny
        logovaci format.
        """
        if entry is None:
            entry = log_file.readline()
            log_file.seek(0)
        try:
            self.parser = apachelogs.LogParser(apachelogs.COMMON, errors=None)
            _ = self.parser.parse(entry)
            return SupportedHeaders.COMMON
        except apachelogs.InvalidEntryError:
            try:
                self.parser = apachelogs.LogParser(apachelogs.COMBINED, errors=None)
                _ = self.parser.parse(entry)
                return SupportedHeaders.COMBINED
            except apachelogs.InvalidEntryError:
                if re.match("^AnonID\s+Query\s+QueryTime\s+ItemRank\s+ClickURL*$", entry):
                    return SupportedHeaders.QUERY
                try:
                    self.parser = apachelogs.LogParser(apachelogs.VHOST_COMMON)
                    _ = self.parser.parse(entry)
                    return SupportedHeaders.COMMON
                except:
                    return None
            except apachelogs.InvalidEntryError:
                return None
