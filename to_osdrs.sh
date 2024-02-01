./zip_experiment.sh

echo "put ./delivery/$(ls -rt ./delivery/ | tail -n1)" | sftp yasiu@opssat1.esoc.esa.int:/experiments/exp266/toESOC/sepp_packages/
#ssh sandbox "cd ~/sdr/exp266/;rsync --archive --verbose --human-readable --delete * osdrs:/chroot/experiments/exp266/source"
#ssh sandbox "cd ~/sdr/exp266/delivery;rsync --archive --verbose --human-readable --delete \$(ls -rt | tail -n1) osdrs:/chroot/experiments/exp266/toESOC/sepp_packages"