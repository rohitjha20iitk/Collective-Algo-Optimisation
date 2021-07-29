#!/usr/bin/env python
# coding: utf-8

# In[62]:


import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

sns.set()


# In[63]:


f4_1 = open("data_4*1.txt","r")

B4_1 = []
R4_1 = []
G4_1 = []

c=0
for i in f4_1:
    if c%6 == 0:
        B4_1.append(i[5:13])
    elif c%6 == 1:
        B4_1.append(i[5:13])
    elif c%6 == 2:
        R4_1.append(i[5:13])
    elif c%6 == 3:
        R4_1.append(i[5:13])
    elif c%6 == 4:
        G4_1.append(i[5:13])
    else:
        G4_1.append(i[5:13])
    c = c + 1


# In[64]:


f4_8 = open("data_4*8.txt","r")
B4_8 = []
R4_8 = []
G4_8 = []

c=0
for i in f4_8:
    if c%6 == 0:
        B4_8.append(i[5:13])
    elif c%6 == 1:
        B4_8.append(i[5:13])
    elif c%6 == 2:
        R4_8.append(i[5:13])
    elif c%6 == 3:
        R4_8.append(i[5:13])
    elif c%6 == 4:
        G4_8.append(i[5:13])
    else:
        G4_8.append(i[5:13])
    c = c + 1


# In[65]:


f16_1 = open("data_16*1.txt","r")
B16_1 = []
R16_1 = []
G16_1 = []

c=0
for i in f16_1:
    if c%6 == 0:
        B16_1.append(i[5:13])
    elif c%6 == 1:
        B16_1.append(i[5:13])
    elif c%6 == 2:
        R16_1.append(i[5:13])
    elif c%6 == 3:
        R16_1.append(i[5:13])
    elif c%6 == 4:
        G16_1.append(i[5:13])
    else:
        G16_1.append(i[5:13])
    c = c + 1


# In[66]:


f16_8 = open("data_16*8.txt","r")
B16_8 = []
R16_8 = []
G16_8 = []

c=0
for i in f16_8:
    if c%6 == 0:
        B16_8.append(i[5:13])
    elif c%6 == 1:
        B16_8.append(i[5:13])
    elif c%6 == 2:
        R16_8.append(i[5:13])
    elif c%6 == 3:
        R16_8.append(i[5:13])
    elif c%6 == 4:
        G16_8.append(i[5:13])
    else:
        G16_8.append(i[5:13])
    c = c + 1


# In[67]:


Bcast = []
i = 0
for c in range(10):
    for j in range(i,i+6):
        Bcast.append(B4_1[j])
    for j in range(i,i+6):
        Bcast.append(B4_8[j])
    for j in range(i,i+6):
        Bcast.append(B16_1[j])
    for j in range(i,i+6):
        Bcast.append(B16_8[j])
    i = i + 6


# In[68]:


Reduce = []
i = 0
for c in range(10):
    for j in range(i,i+6):
        Reduce.append(R4_1[j])
    for j in range(i,i+6):
        Reduce.append(R4_8[j])
    for j in range(i,i+6):
        Reduce.append(R16_1[j])
    for j in range(i,i+6):
        Reduce.append(R16_8[j])
    i = i + 6


# In[69]:


Gather = []
i = 0
for c in range(10):
    for j in range(i,i+6):
        Gather.append(G4_1[j])
    for j in range(i,i+6):
        Gather.append(G4_8[j])
    for j in range(i,i+6):
        Gather.append(G16_1[j])
    for j in range(i,i+6):
        Gather.append(G16_8[j])
    i = i + 6


# In[70]:


demo_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})


# In[71]:


i = 0
for execution in range(10):
    for P in [4, 16]:
        for ppn in [1, 8]:
            for D in [16, 256, 2048]:
                # Change with the actual data
                demo_input_format = demo_input_format.append({
                    "D": D, "P": P, "ppn": ppn, "mode": 0, "time": float(Gather[i])
                }, ignore_index=True)
                i = i + 1
                demo_input_format = demo_input_format.append({
                    "D": D, "P": P, "ppn": ppn, "mode": 1, "time": float(Gather[i])
                }, ignore_index=True)
                i = i + 1


# In[72]:


demo_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, demo_input_format["P"]), map(str, demo_input_format["ppn"])))

print(demo_input_format)

sns.catplot(x="(P, ppn)", y="time", data=demo_input_format, kind="box", col="D", hue="mode")
plt.show()


