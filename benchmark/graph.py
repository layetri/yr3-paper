import csv
import numpy as np
from matplotlib import pyplot as plt

preprocess = np.array([])
lookup = np.array([])
realtime = np.array([])

render_index = 1
render_keyword = "render"

with open("benchmark/results/latest.txt") as file:
    data = csv.reader(file, delimiter=',')
    for row in data:
        if "preprocess-"+render_keyword in row[0]:
            preprocess = np.append(preprocess, float(row[render_index]))
        elif "lookup-"+render_keyword in row[0]:
            lookup = np.append(lookup, float(row[render_index]))
        elif "realtime-"+render_keyword in row[0]:
            realtime = np.append(realtime, float(row[render_index]))

print("Preprocessing average: " + str(np.average(preprocess)))
print("LUT average: " + str(np.average(lookup)))
print("Realtime average: " + str(np.average(realtime)))

plt.title("Average loop cycle time (ns)")
plt.plot(lookup, color="lightblue", label="Look-up table")
plt.plot(preprocess, color="lightgreen", label="Preprocessing")
plt.plot(realtime, color="orange", label="Real-time rendering")

plt.xlabel("n")
plt.ylabel("Time (ns)")
plt.legend()
plt.show()