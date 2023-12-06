rsync --archive --verbose --human-readable --delete * sandbox:~/sdr/exp266/
ssh sandbox "cd ~/sdr/exp266;rsync --archive --verbose --human-readable --delete * osops:~/yasiu/experiments/exp266"