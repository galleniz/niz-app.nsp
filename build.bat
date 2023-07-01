echo off
echo Deleting the residuals
title
del build
del my-first-app-nx.*
echo Building...
make
pause
echo on
