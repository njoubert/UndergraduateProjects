#!/bin/bash

echo '\n\nCommitting...\n\n'
svn commit -m "Additions to CS184 TA SP09"
ssh njoubert.com "cd /var/www/teaching/cs184_sp09; ls -la; echo '\n\nUpdating...\n\n' ; svn update"
