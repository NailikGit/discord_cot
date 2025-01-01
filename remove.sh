#! /bin/bash

token=$(<../token)
echo $token

sed -i -e "s/$token/YOUR_BOT_TOKEN/g" $1
