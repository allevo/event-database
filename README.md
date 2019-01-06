# Event Database

Event Database is thougth to receive events and run reducers.
Each reducer is a c function that keeps a state and an event and calculates the next state.

The reducer function is stored into a shared object.

A client can query to this database using (JSON line)[http://jsonlines.org/] with TCP connections.

## Compile and Run

In your computer, dl and pthread library has to be installed.

Before starting, we have to compile the codes: 
```
# jansson deps
cd jansson
./configure
make
make check
# a reducer example
cd ../event-database-example
gcc -I"../event-database-sdk" -I"../jansson/src" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"counter.d" -MT"counter.o" -o "counter.o" "counter.c"
gcc -L"../jansson/src" -L"../jansson/src/.libs" -shared -o "libevent-database-example.so"  ./counter.o   -ljansson
# the database
cd ../event-database
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/pipe/pipe.d" -MT"src/pipe/pipe.o" -o "src/pipe/pipe.o" "src/pipe/pipe.c"
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/logger/log.d" -MT"src/logger/log.o" -o "src/logger/log.o" "src/logger/log.c"
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-database.d" -MT"src/event-database.o" -o "src/event-database.o" "src/event-database.c"
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-engine.d" -MT"src/event-engine.o" -o "src/event-engine.o" "src/event-engine.c"
gcc -I"../event-database-sdk" -I"../jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/event-storage.d" -MT"src/event-storage.o" -o "src/event-storage.o" "src/event-storage.c"
gcc -L"../jansson/src/.libs" -o "event-database"  ./src/pipe/pipe.o  ./src/logger/log.o  ./src/event-database.o ./src/event-engine.o ./src/event-storage.o   -lpthread -ldl -ljansson
```

Run:
```
cd event-database
./event-database
```

If you are using Eclipse this repo is a workspace one.

## Query

In order to query to database:
```
$ telnet 127.0.0.1 54321
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
{"type":"R:A","name":"counter","so":"/<path>/<to>/<repo>/event-database-example/libevent-database-example.so"}
"OK"
{"type":"E","name":"some"}
"OK"
{"type":"S:G","name":"counter"}
1
```

Three kind of query can be performed:

### Add new Reducer

Using `type` `R:A`, you can add a reducer. The `so` is the path to shared object library

### Fire an event

Using `type` `E`, you can fire an event into the database.

### Get a reducer state

Using `type` `S:G`, you can retrieve the state of a reducer.

## TODO

This project is born in a weekend. A LOT of piece should be improved.
- Allow multiple client connections
- Store events on FS (started but not finished)
- Add APIs for reducer (remove, avoid duplicated)
- Run reducers on separated threads (pipe can be used for this)
- ...

## Licence

Jansson library has own Licence.
Some piece of codes like "pipe" and "logger" library are published under their licence.

The remain code are published under MIT.

