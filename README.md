# BurstSketch

Note: you can view the updated code in the link (https://github.com/mrj222/burst-sketch), in which `cpu` contains the source code for CPU experiments. 

## Introduction

Burst is a common pattern in data streams which consists of a sudden increase in terms of arrival rate followed by a sudden decrease. The design goal of this paper is to use limited size of memory to accurately detect bursts in real time. Towards the design goal of this paper, we propose a novel sketch to detect bursts in real time, namely BurstSketch. BurstSketch consists of two parts, Stage 1 and Stage 2. In Stage 1, we use the technique Running Track to select potential burst items efficiently. In Stage 2, we monitor the potential burst items and capture the key features of burst pattern by a technique named Snapshotting. We conduct extensive experiments to evaluate the performance of our sketch. Experimental results show that compared to the strawman solution, our sketch improves the recall rate up to 1.75 times.

## About the source code

There are two directories here, one is BurstDetection, another is High-speed. Each one is an application of the BurstSketch.

The source code in each directory is implemented by C++, including the BurstSketch algorithm and the strawman solution.

You should use your own dataset and move it to the root directory.

## How to run

If you have already cloned the repository, you need to get the datasets and move it to the root directory. Then you need to change the path of the dataset in param.h. Finally, you just need to compile and run main.cpp.

## Input format

You do not need to input anything.

You can change the parameters in either main.cpp or param.h.

## Output format

We output some metrics for each application. (like ARE, CR, PR, etc)