

gcc init_db.c -o initdb `mysql_config --cflags --libs`
./initdb root
rm initdb
gcc -w -pthread threads.c -o threads
gcc -w -pthread process.c -o processes
gcc -w -pthread gui.c -o main `pkg-config --cflags --libs gtk+-2.0` `mysql_config --cflags --libs`