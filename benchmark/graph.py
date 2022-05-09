import csv
from curses import has_key
from datetime import datetime
import os
import numpy as np
from matplotlib import pyplot as plt

dt = datetime.now().strftime("%m-%d-%Y_%H-%M-%S")

def loadData(name, property):
    d = np.array([])
    with open("benchmark/results/latest.txt") as file:
        data = csv.reader(file, delimiter=',')
        for row in data:
            if name in row[0]:
                d = np.append(d, float(row[property]))
    
    return d

def loadLinearityScale():
    return loadData("start-q", 5)

def graph(data, title, filename, labelx="n", labely="Time (ns)", xval=[]):    
    script_dir = os.path.dirname(__file__)
    results_dir = os.path.join(script_dir, "results/" + dt)

    if not os.path.isdir(results_dir):
        os.makedirs(results_dir)

    plt.title(title)
    for d in data:
        if "xval" in d:
            plt.plot(d["xval"], d["data"], color=d["color"], label=d["label"])
        else:
            plt.plot(d["data"], color=d["color"], label=d["label"])
        
        print("[" + filename + "] " + d["label"] + " average: " + str(np.average(d["data"])))

    plt.xlabel(labelx)
    plt.ylabel(labely)
    plt.legend()
    plt.savefig(results_dir + "/" + filename + ".png")
    plt.close()

# Calculate envelope stats
i=1
preprocess = {'data': loadData("preprocess-init", i), 'color': 'lightgreen', 'label': 'Preprocessing'}
lookup = {'data': loadData("lookup-init", i), 'color': 'lightblue', 'label': 'Look-up table'}
realtime = {'data': loadData("realtime-init", i), 'color': 'orange', 'label': 'Real-time rendering'}
graph([preprocess, lookup, realtime], "Initialization time (ns)", "env_init_time")

i=1
preprocess = {'data': loadData("preprocess-render", i), 'color': 'lightgreen', 'label': 'Preprocessing'}
lookup = {'data': loadData("lookup-render", i), 'color': 'lightblue', 'label': 'Look-up table'}
realtime = {'data': loadData("realtime-render", i), 'color': 'orange', 'label': 'Real-time rendering'}
graph([preprocess, lookup, realtime], "Average render cycle time (ns)", "env_cycle_time")

print("Preprocessing average: " + str(np.average(preprocess["data"])))
print("LUT average: " + str(np.average(lookup["data"])))
print("Realtime average: " + str(np.average(realtime["data"])))

# Calculate scheduler stats
i=1
deterministic = {'data': loadData("deterministic-loop", i), 'color': 'lightgreen', 'label': 'Deterministic'}
stochastic = {'data': loadData("start-loop", i), 'color': 'lightblue', 'label': 'Random start'}
stochastic2 = {'data': loadData("chance-loop", i), 'color': 'orange', 'label': 'Random chance'}
divdown = {'data': loadData("dividedown-loop", i), 'color': 'pink', 'label': 'Divide-down'}
# graph([deterministic, stochastic, stochastic2, divdown], "Average render cycle time (ns)", "scd_cycle_time")
graph([deterministic, stochastic, divdown], "Average render cycle time (ns)", "scd_cycle_time")

i=3
deterministic = {'data': loadData("deterministic-q", i), 'color': 'lightgreen', 'label': 'Deterministic'}
stochastic = {'data': loadData("start-q", i), 'color': 'lightblue', 'label': 'Random start'}
stochastic2 = {'data': loadData("chance-q", i), 'color': 'orange', 'label': 'Random chance'}
divdown = {'data': loadData("dividedown-q", i), 'color': 'pink', 'label': 'Divide-down'}
graph([deterministic, stochastic, stochastic2, divdown], "Coverage (%)", "scd_coverage", "n", "Percentage (%)")

i=5
deterministic = {'data': loadData("deterministic-q", i), 'color': 'lightgreen', 'label': 'Deterministic', 'xval': loadData("deterministic-q", i-1)}
stochastic = {'data': loadData("start-q", i), 'color': 'lightblue', 'label': 'Random start', 'xval': loadData("start-q", i-1)}
stochastic2 = {'data': loadData("chance-q", i), 'color': 'orange', 'label': 'Random chance', 'xval': loadData("chance-q", i-1)}
divdown = {'data': loadData("dividedown-q", i), 'color': 'pink', 'label': 'Divide-down', 'xval': loadData("dividedown-q", i-1)}
graph([deterministic, stochastic, stochastic2, divdown], "User input linearity: density", "scd_linearity", "Input", "Output")
