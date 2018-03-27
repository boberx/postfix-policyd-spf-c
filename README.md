# postfix-policyd-spf-c
# Description
Sender Policy Framework in a Postfix  
This program is based on Matthias Cramer's policyd-spf-fs  
  
(very old project, tested with Debian 6/7 on amd64 and postfix 2.7/2.11)  
# How to use

master.cf  
  
```
postfix_spfpd   unix    -       n       n       -       -       spawn user=nobody argv=/usr/sbin/postfix-policyd-spf-c -w /etc/postfix/filter/spfwhite -d 1
```
