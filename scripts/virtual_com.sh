#!/bin/bash

sudo screen -dmS socat_session socat -d -d pty,rawer,echo=0,link=/dev/virtualcom0 pty,rawer,echo=0,link=/dev/virtualcom1
sudo chmod 666 /dev/virtualcom0
sudo chmod 666 /dev/virtualcom1
