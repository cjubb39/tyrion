#!/sbin/ash

/sbin/mark_app

/sbin/insmod svd.ko
/sbin/mdev -s
./svd-mmap &
wait
/sbin/rmmod svd

/sbin/qsim_exit
