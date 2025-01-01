#! /bin/bash

token=$(<../token)
echo $token

sed -i -e "s/YOUR_BOT_TOKEN/$token/g" $1
