
set -ax


if [ ! -f jansson/src/jansson.h ]; then
	rm -rf jansson jansson-2.12
	tar -xzf jansson-2.12.tar.gz
	mv jansson-2.12 jansson
	# jansson deps
	cd jansson
	./configure
	make
	make check
else
	cd jansson
fi
# a reducer example
cd ../event-database-example
gcc -I"../event-database-sdk" -I"../jansson/src" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"counter.d" -MT"counter.o" -o "counter.o" "counter.c" -std=c99
gcc -L"../jansson/src" -L"../jansson/src/.libs" -shared -o "libevent-database-example.so"  ./counter.o   -ljansson -std=c99
# the database
cd ../event-database
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/pipe/pipe.d" -MT"src/pipe/pipe.o" -o "src/pipe/pipe.o" "src/pipe/pipe.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/logger/log.d" -MT"src/logger/log.o" -o "src/logger/log.o" "src/logger/log.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-database.d" -MT"src/event-database.o" -o "src/event-database.o" "src/event-database.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-engine.d" -MT"src/event-engine.o" -o "src/event-engine.o" "src/event-engine.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-storage.d" -MT"src/event-storage.o" -o "src/event-storage.o" "src/event-storage.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-net.d" -MT"src/event-net.o" -o "src/event-net.o" "src/event-net.c" -std=c99
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-parser.d" -MT"src/event-parser.o" -o "src/event-parser.o" "src/event-parser.c" -std=c99

gcc -L"../jansson/src" -L"../jansson/src/.libs" -o "event-database"  ./src/pipe/pipe.o  ./src/logger/log.o  ./src/event-database.o ./src/event-engine.o ./src/event-net.o ./src/event-parser.o ./src/event-storage.o   -ljansson -lpthread -ldl

gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/tests/tests.d" -MT"src/tests/tests.o" -o "src/tests/tests.o" "src/tests/tests.c" -std=c99

gcc -L"../jansson/src" -L"../jansson/src/.libs" -o "event-database-tests"  ./src/pipe/pipe.o  ./src/logger/log.o ./src/event-engine.o ./src/event-net.o ./src/event-parser.o ./src/event-storage.o ./src/tests/tests.o  -ljansson -lpthread -ldl
./event-database-tests
