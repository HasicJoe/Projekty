#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import zipfile
import requests
from bs4 import BeautifulSoup
import os
import csv
import io
import gzip
import pickle


class DataDownloader:
    """ 
    Downloads traffic accident data to specified folder and then parses this data into a dictionary.

    Attributes:
        headers         Names of the headers of individual CSV files
        regions         Dictionary with the names of the region : name of the CSV file
        float_columns   List of columns that contain values of float type
        str_columns     List of columns that contain values of str type
    """

    headers = ["p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13a",
               "p13b", "p13c", "p14", "p15", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27", "p28",
               "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53", "p55a",
               "p57", "p58", "a", "b", "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q", "r", "s", "t", "p5a"]

    regions = {
        "PHA": "00",
        "STC": "01",
        "JHC": "02",
        "PLK": "03",
        "ULK": "04",
        "HKK": "05",
        "JHM": "06",
        "MSK": "07",
        "OLK": "14",
        "ZLK": "15",
        "VYS": "16",
        "PAK": "17",
        "LBK": "18",
        "KVK": "19",
    }
    
    float_columns = [45, 46, 47, 48, 49, 50, 57]
    str_columns = [51, 52, 53, 54, 55, 56, 58, 59, 62, 64]

    def __init__(self, url="https://ehw.fit.vutbr.cz/izv/", folder="data", cache_filename="data_{}.pkl.gz"):
        """
        Constructs necessary attributes for the DataDownloader object

        Attributes:
            url             Specifies from which url address the data is read
            folder          Determines where the data of traffic accidents is stored
            cache_filename  Filename which determines where the processed data from get_dict() method will be stored
        """
        self.url = url
        self.folder = folder
        self.cache_filename = cache_filename
        self.links = list()
        self.cached = dict()

    def download_data(self):
        """
        Downloads all necessary data from the url and store them to the specified folder.
        """
        response = requests.get(self.url)
        if response.status_code == 200:
            soup = BeautifulSoup(response.text, "html.parser")
            self.get_links(soup.body.table.find_all("tr"))
            for link in self.links:
                filepath = os.path.join(
                    os.getcwd(), self.folder, link.split("/", maxsplit=1)[1]
                )
                if not os.path.exists(filepath):
                    r = requests.get(f"{self.url}/{link}", stream=True)
                    with open(filepath, "wb") as f:
                        f.write(r.content)

    def get_links(self, table_rows) -> list:
        """
        Returns last button onclick link from each row.
        
        Attributes:
            table_rows      All <tr> tags of HTML <table> tag
        """
        for row in table_rows:
            self.links.append(self.parse_link(row.find_all("button")[-1]))

    def parse_link(self, tag) -> str:
        """
        Obtains and processes value from HTML tag.

        Attributes:
            tag     HTML button tag
        """
        attr_value = tag.get("onclick")
        return attr_value[(attr_value.find("(") + 2) : (attr_value.find(")") - 1)]

    def init_str_max(self) -> dict:
        """
        Initializes and sets dict values that monitor max length of str columns.
        """
        str_max = dict()
        for i in DataDownloader.str_columns:
            str_max[f"{i}_max"] = 0
        return str_max

    def parse_region_data(self, region):
        """
        If region data is not downloaded, downloads it otherwise only processes region data and save them into dict.

        Attributes:
            region  Certain CR region, specified by three-character (one of the keys from class dictionary - regions)
        """
        if not os.path.isdir(self.folder):
            dirs = self.folder.split("/")
            path = ""
            for dir in dirs:
                if not path:
                    path = os.path.join(f"{os.getcwd()}/{dir}")
                else:
                    path = os.path.join(f"{path}/{dir}")
                if not os.path.isdir(path):
                    os.makedirs(path)
        # check if we already download all files
        downloaded_years = [
            f"{self.folder}/{fnm}"
            for fnm in os.listdir(self.folder)
            if os.path.isfile(os.path.join(self.folder, fnm))
            and zipfile.is_zipfile(os.path.join(self.folder, fnm))
        ]
        if len(downloaded_years) < 6:
            self.download_data()
            downloaded_years = [
                f"{self.folder}/{fnm}"
                for fnm in os.listdir(self.folder)
                if os.path.isfile(os.path.join(self.folder, fnm))
                and zipfile.is_zipfile(os.path.join(self.folder, fnm))
            ]

        region_file = DataDownloader.regions.get(region)
        data = self.init_dict()
        str_column_max = self.init_str_max()
        source = list()
        for i in range(len(DataDownloader.headers) + 1):
            source.append([])

        for year in downloaded_years:
            with zipfile.ZipFile(year, "r") as zip_f:
                with zip_f.open(f"{region_file}.csv", "r") as csv_f:
                    reader = csv.reader(
                        io.TextIOWrapper(csv_f, "cp1250"), delimiter=";", quotechar='"'
                    )
                    for row in reader:
                        row.append(str(region))
                        row[3] = row[3].replace("-", "")
                        for i, col_value in enumerate(row):
                            if i in DataDownloader.float_columns:
                                row[i] = row[i].replace(",", ".")
                                if col_value.isnumeric():
                                    source[i].append(float(col_value))
                                else:
                                    source[i].append(float(0))
                            elif i in DataDownloader.str_columns:
                                if len(col_value) > str_column_max[f"{i}_max"]:
                                    str_column_max[f"{i}_max"] = len(col_value)
                                if col_value:
                                    source[i].append(str(col_value))
                                else:
                                    source[i].append("")
                            else:
                                if col_value.isnumeric():
                                    source[i].append(int(col_value))
                                else:
                                    source[i].append(int(0))
        return self.convert_data(source, data, str_column_max)

    def init_dict(self) -> dict:
        """
        Initializes keys for the specific dictionary, which is used for region data processing.
        """
        l = DataDownloader.headers
        l.append(str("region"))
        return dict.fromkeys(l)

    def get_column_max(self, index, column_max):
        """
        Returns the max value of a certain str column increased by 1.
        """
        return column_max.get(f"{index}_max") + 1

    def convert_data(self, source, target, str_column_max) -> dict:
        """
        Converts acquired and cleaned data (list), to a dictionary with numpy arrays.

        Attributes:
            source          List with acquired data
            target         Output dictionary, where the acquired data will be stored.
            str_column_max  Dictionary which determines str size for numpy array.
        """
        key_list = list(target.keys())
        unique_id = key_list.pop(0)
        np_a = np.array(source[0])
        _, indices = np.unique(np_a, return_index=True)
        target[unique_id] = np_a[indices]
        not_unique = len(source[0]) - len(indices)

        for i, k in enumerate(key_list):
            if i + 1 in DataDownloader.float_columns:
                if not_unique > 0:
                    np_arr = np.array(source[i + 1], dtype=float)
                    target[k] = np_arr[indices]
                else:
                    target[k] = np.array(source[i + 1], dtype=float)
            elif i + 1 in DataDownloader.str_columns:
                str_len = self.get_column_max(i + 1, str_column_max)
                if not_unique > 0:
                    np_arr = np.array(source[i + 1], dtype=f"<U{str_len}")
                    target[k] = np_arr[indices]
                else:
                    target[k] = np.array(source[i + 1], dtype=f"<U{str_len}")
            else:
                if not_unique > 0:
                    np_arr = np.array(source[i + 1], dtype=int)
                    target[k] = np_arr[indices]
                else:
                    target[k] = np.array(source[i + 1], dtype=int)
        return target

    def get_cache_name(self, region) -> str:
        """
        Returns name of cache file(str).
        """
        return f'{self.folder}/{self.cache_filename.replace("{}", region)}'

    def concat_np_arrays(self, target, source) -> dict:
        """
        Concatenates two numpy arrays into target dictionary.

        Attributes:
            target      Dictionary in which the concatenated array is sotred.
            source      Dictionary that will be concatenated with target array.
        """
        for k in target.keys():
            target[k] = np.concatenate((target[k], source[k]), axis=0)
        return target

    def get_dict(self, regions=None):
        """
        Processes and returns data for specified regions.

        Attributes:
            regions     Specifies the regions for which we want to return the result [str|list|None]
        """
        if isinstance(regions, list):
            if not regions:
                regions = list(DataDownloader.regions.keys())
            else:
                # filter invalid values out of list
                regions = [
                    reg for reg in regions if reg in DataDownloader.regions.keys()
                ]
        elif isinstance(regions, str):
            if not regions in DataDownloader.regions.keys():
                regions = DataDownloader.regions.keys()
            else:
                regions = [regions]
        elif regions is None:
            regions = list(DataDownloader.regions.keys())

        #first region so we dont have data stored into class variable cached.
        if os.path.exists(self.get_cache_name(regions[0])):
            with gzip.open(self.get_cache_name(regions[0]), "rb") as pickle_f:
                self.cached[regions[0]] = pickle.load(pickle_f)
            data = self.cached[regions[0]]
        else:
            data = self.parse_region_data(regions[0])
            with gzip.open(self.get_cache_name(regions[0]), "wb", compresslevel=6) as pickle_f:
                pickle.dump(data, pickle_f)

        if len(regions) > 1:
            for region in regions[1::]:
                if region in self.cached.keys():
                    data = self.concat_np_arrays(data, self.cached[region])
                elif os.path.exists(self.get_cache_name(region)):
                    with gzip.open(self.get_cache_name(region), "rb") as pickle_f:
                        self.cached[region] = pickle.load(pickle_f)
                    data = self.concat_np_arrays(data, self.cached[region])

                else:
                    next_data = self.parse_region_data(region)
                    # serialisation to the file
                    with gzip.open(self.get_cache_name(region), "wb", compresslevel=6) as pickle_f:
                        pickle.dump(next_data, pickle_f)
                    data = self.concat_np_arrays(data, next_data)
        return data


if __name__ == "__main__":

    regs = ["PHA", "ULK", "KVK"]
    data_downloader = DataDownloader().get_dict(regs)
    # Write basic stats
    traf_len = len(data_downloader["p1"])
    columns = list(data_downloader.keys())
    # little validity check
    for column in columns:
        assert len(data_downloader[column]) == traf_len 
    col_len = len(columns)
    print(f"Processed regions: {regs}")
    print(f"Data columns: {columns}")
    print(f"Number of processed columns: {col_len}")
    for reg in regs:
        r_l = len(np.where(data_downloader['region'] == reg)[0])
        print(f"Number of evidence for {reg} region: {r_l}")
    print(f"Total number of traffic evidences: {traf_len}")
    
