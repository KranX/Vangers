#!/bin/bash

# start websockify as daemon
websockify --cert $CER_FILE --key $KEY_FILE $SERVER:2198 $SERVER:2197 -D
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to start websockify: $status"
  exit $status
fi

# start the vangers server
./bin/server