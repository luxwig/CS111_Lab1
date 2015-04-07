cat > test.h << EOF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EOF

while IFS= read -r line
do
a=`grep "${line}" read-command.c  |sed 's://.*::'`
a+=';'
echo "$a" >> test.h
done
