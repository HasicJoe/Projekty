#!/usr/bin/python3.8
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np
# muzete pridat vlastni knihovny


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """
    Convert pandas DataFrame to geopandas DataFrame with correct encoding.
    Attributes:
        df      pandas dataframe
    """
    geo_df = geopandas.GeoDataFrame(df, geometry=geopandas.points_from_xy(df['d'], df['e']), crs='EPSG:5514')
    return geo_df[(~geo_df['d'].isnull()) & (~geo_df['e'].isnull())]
    

def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """
    Plot the graph with six subplots according to the location of the accident for specific region.
    Attributes:
        gdf             geopandas GeoDataFrame which contains data about traffic accidents
        fig_location    If set, the graph image is saved to the path specified by this argument.
        show_figure     If set, the graph is displayed in the window.
    """
    fix, ax = plt.subplots(3, 2, figsize=(14, 9.5))
    gdf["date"] = gdf["p2a"].astype("datetime64")
    region = "JHM"
    gdf = gdf[gdf['region'] == region]
    # calculate limits for axis
    limits = gdf.total_bounds
    years = [2018, 2019, 2020]
    for i in range(3):
        # Plot left one
        gdf[(gdf['p36'] == 0) & (gdf['date'].dt.year == years[i])].plot(ax = ax[i, 0], markersize=1, color="tab:red")
        ax[i, 0].set_title(f"Kraj: {region}, dialnice ({years[i]})")
        ax[i, 0].set_axis_off()
        ax[i, 0].set_xlim(limits[0], limits[2])
        ax[i, 0].set_ylim(limits[1], limits[3])
        ctx.add_basemap(ax[i, 0], crs=gdf.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
        # Plot right one
        gdf[(gdf['p36'] == 1) & (gdf['date'].dt.year == years[i])].plot(ax = ax[i, 1], markersize=1)
        ax[i, 1].set_title(f"Kraj: {region}, cesty prvej triedy ({years[i]})")
        ax[i, 1].set_axis_off()
        ax[i, 1].set_xlim(limits[0], limits[2])
        ax[i, 1].set_ylim(limits[1], limits[3])
        ctx.add_basemap(ax[i, 1], crs=gdf.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
    
    # Remove ticks
    plt.tick_params(bottom=False, top=False, left=False, right=False)
    plt.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ 
    Plot a graph with the location of all accidents in the region clustered into a cluster.
    Attributes:
        gdf             geopandas GeoDataFrame which contains data about traffic accidents
        fig_location    If set, the graph image is saved to the path specified by this argument.
        show_figure     If set, the graph is displayed in the window.
    """
    region = "JHM"
    # Filter by region & 1.st class roads
    gdf = gdf.loc[(gdf['region'] == region) & (gdf['p36'] == 1), :] 
    coordinates = np.dstack([gdf.geometry.x, gdf.geometry.y]).reshape(-1, 2)
    # postupnym experimentovanim som sa dostal k 21 clusterom, ktore sa najviac zhoduju s vzorovym riesenim
    # pri nizkom pocte clusterov je narocne vyznacit kriticke useky ciest 1. triedy
    # pri privelmi vysokom pocte clusterov je vysledok totozny ako pri prilis nizkom pocte
    db = sklearn.cluster.MiniBatchKMeans(n_clusters=21).fit(coordinates)
    gdf_c = gdf.copy()
    gdf_c['cluster'] = db.labels_
    gdf_c['count'] = 0
    gdf_c = gdf_c.dissolve(by='cluster', aggfunc={'count' : 'count'})
    fig, ax = plt.subplots(1, 1, figsize=(14, 8.5))
    legend_settings = {'orientation' : 'horizontal', 'label' : 'Počet nehôd v úseku', 'shrink': 0.65, 'pad' : 0.02}
    gdf_c.plot(ax=ax, column='count', legend=True, markersize=1.4, legend_kwds=legend_settings)
    ctx.add_basemap(ax=ax, crs=gdf_c.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
    ax.set_axis_off()
    ax.set_title(f"Kraj: {region}, nehody na cestach 1. triedy")
    plt.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()
    

if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
