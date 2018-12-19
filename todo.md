# Include guards and -include option change expected results

Problem:
It was expected that -H option would show all includes in hierarchical style. 
But it seems that -H option shows only those includes that were processed by preprocessor. That is for next code:
```c
#include <stdio.h>
#include <stdio.h>
```

-H option will show only one include because stdio.h uses include guard. So the second include will not be precessed by preprocessor.

About -include option. Int test for current commit -H does not show stdio.h as include:

```c
#include <stdio.h>
#include "foo1.h"

void main ()
{
	foo1();
	foo3();
}
```

```bash
alex@alex (16:09:23):~/Downloads/git/include-explorer/test $ gcc -c -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/main.c -H
. include/foo1/foo1.h
.. include/foo2/foo2.h
Multiple include guards may be useful for:
././include/foo3/foo3.h
/usr/include/wchar.h
/usr/include/x86_64-linux-gnu/bits/stdio_lim.h
/usr/include/x86_64-linux-gnu/bits/sys_errlist.h
/usr/include/x86_64-linux-gnu/bits/typesizes.h
/usr/include/x86_64-linux-gnu/gnu/stubs-64.h
/usr/include/x86_64-linux-gnu/gnu/stubs.h
include/foo1/foo1.h
include/foo2/foo2.h
```
Removing -include option shows stdio.h with warnings about foo3.
It seems that preprocessor takes foo3.h, preprocess it (and removes all includes) and paste the result at the start of the main.c. So when preprocessor starts to preprocess main.c, main.c already has all code to be compiled successfully. So -H does not show files included in foo3.h and shows only foo1.h and foo2.h. It also does not show stdio.h because of include guards that were added after inserting foo3.h contents.

Solution:
	- Include guards can not be fixed. So we will see includes that are needed for some file but it is possible that include file will be inside another file not in target file. Include file from target file will be discarded because of include guards.
	- -include option can be removed and file from -include option will be moved to the first line of target file.

#Macro #error vs make -f tmpMakefile
make -f tmpMakefile faile while compile python:

```bash
alex@alex:~/output/build/host-python-markupsafe-1.0$ cd /home/alex/output/build/host-python-markupsafe-1.0 && /home/alex/output/host/bin/arm-linux-gnueabihf-gcc -pthread -fno-strict-aliasing -O2 -I/home/alex/output/host/include -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -fPIC -I/home/alex/output/host/include/python2.7 /home/alex/output/build/host-python-markupsafe-1.0/markupsafe/_speedups.c -H -fsyntax-only
```

```bash
In file included /output/host/include/python2.7/Python.h:61:0,
                 /output/build/host-python-markupsafe-1.0/markupsafe/_speedups.c:12:
/output/host/include/python2.7/pyport.h:895:2: warning: #warning "LONG_BIT definition appears wrong for platform (bad gcc/glibc config?)." [-Wcpp]
 #error "LONG_BIT definition appears wrong for platform (bad gcc/glibc config?)."
  ^~~~~~~
```

Don't know how to fix that. This error make 'make -f tmpMakefile' to exit. If -H is not used, but -MM -MG are used - there is no such error, but output is different.