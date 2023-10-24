import numpy as np 
import pandas as pd
import os 
import matplotlib.pyplot as plt 

def load_data(name):
    return pd.read_csv(name)

def make_figure(data_list, file_names):
    fig, ax = plt.subplots(figsize=(10,10), dpi=150)
    markers = [
    'k-*', 'g-*', 'r-*', 'k--.', 'r--.', 'g--.', 'r--*',
    'c-+', 'm-s', 'y-d', 'b-.p', 'm-.h', 'b:x', 'g-2',
    'k--o', 'r-.^', 'c-1', 'm-3', 'b-.', 'g--D', 'k-.',
    'b:D', 'g:^', 'k-+', 'r:o', 'm-2', 'c-v', 'b-1',
    'y:s', 'g->', 'r-h', 'c-4', 'm-.', 'b-o', 'g-+',
    'k-*', 'r--.', 'c-+', 'm-s', 'y-d', 'b-.p', 'm-.h',
    'b:x', 'g-2', 'k--o', 'r-.^', 'c-1', 'm-3', 'b-.',
    'g--D', 'k-.', 'b:D', 'g:^', 'k-+', 'r:o', 'm-2'
    ]

    for i, data in enumerate(data_list):
        ax.plot(data['recall_at_10'], data['cnt_per_second'], markers[i], label=file_names[i].split('/')[-1])

    ax.set_ylabel('Queries Per Second', fontsize=25)
    ax.set_xlabel('Recall@10', fontsize=25)



    ax.tick_params(axis='x', labelsize=22)
    ax.tick_params(axis='y', labelsize=22)



    ax.set_xlim(0.9, 1.0)
    ax.set_ylim(0, 13000)
    fig.tight_layout()
    ax.legend(fontsize = 20)

    fig.savefig('./out_fig.png', dpi = 300)

if __name__ == "__main__":


    file_names = os.listdir('./nns_records/')
    res_data = [
        load_data('./nns_records/' +name) for name in file_names
    ]

    make_figure(res_data, file_names)