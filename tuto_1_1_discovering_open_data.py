
import gwosc
#print(gwosc.__version__)
from gwosc.datasets import find_datasets
from gwosc import datasets
from gwosc.datasets import event_gps
from gwosc.datasets import run_segment
from gwpy import time as tp
from gwosc.datasets import run_segment
from gwpy import time
from gwpy.timeseries import TimeSeries
import matplotlib
import warnings
warnings.filterwarnings("ignore", "Wswiglal-redir-stdio")
#-- List all available catalogs
# print("List of available catalogs")
# print(find_datasets(type="catalog"))
# print("")
#-- Print all the GW events from the GWTC-1 catalog
# gwtc1 = datasets.find_datasets(type='events', catalog='GWTC-1-confident')
# print('GWTC-1 events:', gwtc1)
# print("")
# gwtc2 = datasets.find_datasets(type= 'events', catalog ='GWTC-2.1-confident')
# print('GWTC-2 events:',gwtc2)
"""Note that the event name is of the type _GWyymmdd-vx_ where _x_ is the last available version for the data set provided by GWOSC."""
# #-- Print all the large strain data sets from LIGO/Virgo observing runs
# runs = find_datasets(type='run')
# print('Large data sets:', runs)
# """_Attention: Note that the most recent observation runs, e.g. O2, are labeled with names containing the name of the run (e.g. O2), the sampling rate (4 or 16 kHz) and the release version (e.g. R1). This means that for O2 you have two labels 'O2_4KHZ_R1' and 'O2_16KHZ_R1', depending which is the desired sampling rate_
# `datasets.find_datasets` also accepts a `segment` and `detector` keyword to narrow results based on GPS time and detector:
# """
# #-- Detector and segments keywords limit search result
# # print(datasets.find_datasets(type='events', catalog='GWTC-1-confident', detector="L1", segment=(1164556817, 1187733618)))
# """Using `gwosc.datasets.event_gps`, we can query for the GPS time of a specific event (it works also without the version number):"""


# gps = event_gps('GW190425-v2')
# print(gps)

# print(run_segment('O1'))

# print(tp.from_gps(run_segment('O1')[0]))
# print(tp.from_gps(run_segment('O1')[1]))

# O1_events = datasets.find_datasets(type='events', catalog='GWTC-1-confident', segment=run_segment('O1'))
# print(O1_events)





#print(find_datasets(type='run'))
# timeframe = run_segment('O3a_16KHZ_R1')
# total_run = (timeframe[1])-(timeframe[0])
# months = total_run/(3600*24*30)
#print("run lasted for :",months,"months")
O3conf_events = datasets.find_datasets(type='events', catalog='GWTC-2.1-confident', segment=run_segment('O3a_16KHZ_R1'))
#print(find_datasets(type="catalog"))
# print(len(O3conf_events))
#print(O3conf_events)
gpslist = [0]*len(O3conf_events)
ct=0
for i in O3conf_events:
    gps= event_gps(i)
    gpslist[ct]=gps
    ct=ct+1
ct=0
#print(gpslist)



segment = (int(gpslist[5])-5, int(gpslist[5])+5)
print(segment)
hdata = TimeSeries.fetch_open_data('H1', *segment, verbose=True)
print(hdata)
plot = hdata.plot()
plot.show()


