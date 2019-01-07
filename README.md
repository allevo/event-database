# Event Database

[![Build Status](https://travis-ci.org/allevo/event-database.svg?branch=master)](https://travis-ci.org/allevo/event-database)

Event Database is thougth to receive events and run reducers.
Each reducer is a c function that keeps a state and an event and calculates the next state.

The reducer function is stored into a shared object.

A client can query to this database using [JSON line](http://jsonlines.org/) with TCP connections.

## Compile and Run

In your computer, dl and pthread library has to be installed.

Before starting, we have to compile the codes: 
```
./build.sh
```

Run:
```
cd event-database
./event-database
```

If you are using Eclipse this repo is a workspace.

## Query

In order to query to database:
```
$ telnet 127.0.0.1 54321
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.

{"type":"R:A","name":"counter","so":"/<path>/<to>/<repo>/event-database-example/libevent-database-example.so","rfn":"example_counter","sfn":"setup_example_counter","ffn":"example_get_state_counter"}
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
Some piece of codes like "pipe", "logger", "minunit" libraries are published under their licence.

The remain code are published under MIT.

