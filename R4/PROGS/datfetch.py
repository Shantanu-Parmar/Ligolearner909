import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math
from gwpy.timeseries import TimeSeries

channel = "H1:PEM-VAULT_MAG_1030X195Y_COIL_Y_DQ"

start = int(1238166018)
end =	int(1238170549)
data = TimeSeries.fetch(channel,start,end, host='nds.gwosc.org')
print(data)
data.write("output.gwf")