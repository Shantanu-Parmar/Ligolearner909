import os
import csv
import requests_pelican as rp
from gwdatafind import find_urls
import time 

# Parameters
csv_path = "tim.csv"  # Your input CSV
ifo = "H"
channel_name = "H1_GWOSC_O3a_4KHZ_R1"
host = "https://datafind.gw-openscience.org"
timeout_duration = 2  # Timeout in seconds between each download

# Read CSV and process each time segment
with open(csv_path, newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        start = int(row["start"])
        end = int(row["end"])

        # Find URLs
        urls = find_urls(ifo, channel_name, start, end, urltype='osdf', host=host)

        if not urls:
            print(f"No files found for {start} to {end}")
            continue

        # Create directory structure
        base_dir = f"{channel_name}/{start}_{end}"
        os.makedirs(base_dir, exist_ok=True)

        for url in urls:
            print(f"Downloading: {url}")
            try:
                # Fetch the content of the URL
                content = rp.get(url).content

                # Construct file name and path
                filename = f"{channel_name}_{start}_{end}.gwf"
                filepath = os.path.join(base_dir, filename)

                # Save the content to the file
                with open(filepath, "wb") as f:
                    f.write(content)

                print(f"Saved: {filepath}")
                
                # Add a manual timeout between downloads
                print(f"Waiting for {timeout_duration} seconds before the next download...")
                time.sleep(timeout_duration)  # Simulate a break

            except Exception as e:
                print(f"Failed to download {url}: {e}")
