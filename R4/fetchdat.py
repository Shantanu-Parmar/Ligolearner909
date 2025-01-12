import os
import pandas as pd
from gwpy.timeseries import TimeSeries

# Paths to your CSV files
time_csv_file = "/home/shan909/Desktop/omicron/workinggwf/time.csv"
channel_csv_file = "/home/shan909/Desktop/omicron/workinggwf/channels.csv"

# Load the CSV files
time_ranges = pd.read_csv(time_csv_file, header=None, names=["start", "end"])
channels = pd.read_csv(channel_csv_file, header=None, names=["channel", "sampling_rate"])

# Debug: Check the loaded data
print("Loaded time ranges:")
print(time_ranges)
print("Loaded channels:")
print(channels)

# Path to the fin.txt file
fin_file = "fin.txt"

# Open fin.txt for appending
with open(fin_file, 'a') as fin:

    # Process data for each channel and time range
    for _, channel_row in channels.iterrows():
        channel_name = channel_row["channel"]
        sampling_rate = int(channel_row["sampling_rate"])

        # Create a directory for the channel if it doesn't exist
        channel_dir = channel_name.replace(":", "_")  # Replace ':' with '_'
        os.makedirs(channel_dir, exist_ok=True)

        for _, time_row in time_ranges.iterrows():
            start_time = int(time_row["start"])
            end_time = int(time_row["end"])

            # Fetch data from GWPy
            data = TimeSeries.fetch(channel_name, start=start_time, end=end_time, host='nds.gwosc.org')

            # Create a subfolder for the time range within the channel folder
            time_dir_path = os.path.join(channel_dir, f"{start_time}_{end_time}")
            os.makedirs(time_dir_path, exist_ok=True)

            # File path to save the GWF file
            output_file = os.path.join(
                time_dir_path, f"{channel_name.replace(':', '_')}_{start_time}_{end_time}.gwf"
            )

            # Save the strain data in `gwf` format
            data.write(output_file)
            print(f"Aux data for channel '{channel_name}' from {start_time} to {end_time} saved to {output_file}")

            # Extract relevant data for fin.txt
            t0 = data.t0  # Start time (gps_start_time)
            dt = data.dt  # Duration of the data (file_duration)
            
            # Append the information to fin.txt in the required format
            fin.write(f"{os.path.abspath(output_file)} {t0} {dt} 0 0\n")
            print(f"Added to fin.txt: {os.path.abspath(output_file)} {t0} {dt} 0 0")
