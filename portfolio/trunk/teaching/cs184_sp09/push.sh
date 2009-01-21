#!/bin/bash

svn commit -m "Additions to CS184 TA SP09"
ssh njoubert.com "cd /var/www/teaching/cs184_sp09; svn update"
