#!/sbin/ash

/sbin/mark_app

/sbin/insmod flash.ko
/sbin/mdev -s
./flash-mmap &
wait
/sbin/rmmod flash

/sbin/qsim_exit
