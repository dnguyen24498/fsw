#!/bin/bash

sudo screen -dmS socat_session socat -d -d pty,rawer,echo=0,link=/tmp/virtualcom0 pty,rawer,echo=0,link=/tmp/virtualcom1
sudo chmod 666 /tmp/virtualcom0
sudo chmod 666 /tmp/virtualcom1
