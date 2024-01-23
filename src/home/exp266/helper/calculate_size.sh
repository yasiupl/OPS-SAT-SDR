#!/usr/bin/env sh
sample_size=4 #bytes
sampling_rate=$1 # n/s
number_of_samples=$2

real_sampling=$(($sampling_rate/2))
length=$(($number_of_samples/$real_sampling)) # s
size=$(($(($number_of_samples*$sample_size))/$((1024*1024)))) # MB

echo "Recording at $sampling_rate Samples per seconds and $number_of_samples samples will last ${length} seconds and weigh ${size}MB"