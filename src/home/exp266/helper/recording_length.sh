#!/usr/bin/env sh

sample_size=4 #bytes
sampling_rate=$1 # n/s
number_of_samples=$2 # n

length = number_of_samples / sampling_rate # s
size = number_of_samples * sample_size / 1024 / 1024 # MB