#!/bin/python3.9

# script requirements:
#   > first string must contains path to interpreter
#   > script must be executable (chmod +x script.py) 

import sys
# commandline args : sys.argv ( type = list )
# return code      : ses.exit ( type = int  ) 

# first arg = always would be the scriptname
if len(sys.argv):
  print("script name :", sys.argv[0])
  sys.argv.pop(0)

# other args came from caller and might be parsed
for arg in sys.argv:
  print("            :", arg)

sys.exit(0)