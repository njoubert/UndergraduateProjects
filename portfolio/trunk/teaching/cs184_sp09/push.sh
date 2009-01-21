#!/bin/bash

echo.
echo 'Committing...'
echo.
svn commit -m "Additions to CS184 TA SP09"
ssh njoubert.com "cd /var/www/teaching/cs184_sp09;  echo 'Updating...' ; svn update; ls -la"
