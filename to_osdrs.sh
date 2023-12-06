./zip_experiment.sh
rsync --archive --verbose --human-readable --delete * sandbox:~/sdr/exp266/
ssh sandbox "cd ~/sdr/exp266/delivery;rsync --archive --verbose --human-readable --delete \$(ls -t | tail -n1) osdrs:/chroot/experiments/exp266/toESOC/sepp_packages"