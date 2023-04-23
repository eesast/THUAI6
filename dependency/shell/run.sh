#!/usr/local

nice -10 ./Server/Server --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600
sleep 5
cd /usr/local/team1
if [-f "./capi1"];then
nice -0 ./capi1 -I 127.0.0.1 -P 8888 -p 0
fi
if [-f "./capi2"];then
nice -0 ./capi2 -I 127.0.0.1 -P 8888 -p 1
fi
if [-f "./capi3"];then
nice -0 ./capi3 -I 127.0.0.1 -P 8888 -p 2
fi
if [-f "./capi4"];then
nice -0 ./capi4 -I 127.0.0.1 -P 8888 -p 3
if [-f "./capi5"];then
nice -0 ./capi5 -I 127.0.0.1 -P 8888 -p 4
if [-f "./python/player1.py"];then
nice -0 python3 ./python/player1.py -I 127.0.0.1 -P 8888 -p 0
fi
if [-f "./python/player2.py"];then
nice -0 python3 ./python/player2.py -I 127.0.0.1 -P 8888 -p 1
fi
if [-f "./python/player3.py"];then
nice -0 python3 ./python/player3.py -I 127.0.0.1 -P 8888 -p 2
fi
if [-f "./python/player4.py"];then
nice -0 python3 ./python/player4.py -I 127.0.0.1 -P 8888 -p 3
fi
if [-f "./python/player5.py"];then
nice -0 python3 ./python/player5.py -I 127.0.0.1 -P 8888 -p 4
fi
cd /usr/local/team2
if [-f "./capi1"];then
nice -0 ./capi1 -I 127.0.0.1 -P 8888 -p 0
fi
if [-f "./capi2"];then
nice -0 ./capi2 -I 127.0.0.1 -P 8888 -p 1
fi
if [-f "./capi3"];then
nice -0 ./capi3 -I 127.0.0.1 -P 8888 -p 2
fi
if [-f "./capi4"];then
nice -0 ./capi4 -I 127.0.0.1 -P 8888 -p 3
if [-f "./capi5"];then
nice -0 ./capi5 -I 127.0.0.1 -P 8888 -p 4
if [-f "./python/player1.py"];then
nice -0 python3 ./python/player1.py -I 127.0.0.1 -P 8888 -p 0
fi
if [-f "./python/player2.py"];then
nice -0 python3 ./python/player2.py -I 127.0.0.1 -P 8888 -p 1
fi
if [-f "./python/player3.py"];then
nice -0 python3 ./python/player3.py -I 127.0.0.1 -P 8888 -p 2
fi
if [-f "./python/player4.py"];then
nice -0 python3 ./python/player4.py -I 127.0.0.1 -P 8888 -p 3
fi
if [-f "./python/player5.py"];then
nice -0 python3 ./python/player5.py -I 127.0.0.1 -P 8888 -p 4
fi