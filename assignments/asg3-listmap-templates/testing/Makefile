# $Id: Makefile,v 1.27 2021-10-26 22:04:38-07 - - $
# Cody Yiu (coyiu@ucsc.edu)
# Andrew Lim (ansclim@ucsc.edu)

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless check lint
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory

GPPWARN     = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -std=gnu++2a -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++2a -MM ${GPPOPTS}
UTILBIN     = /afs/cats.ucsc.edu/courses/cse111-wm/bin

MODULES     = listmap xless xpair debug util main
CPPSOURCE   = ${wildcard ${MODULES:=.cpp}}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
SOURCELIST  = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
ALLSOURCE   = ${wildcard ${SOURCELIST}}
EXECBIN     = keyvalue
OTHERS      = ${MKFILE} ${DEPFILE}
ALLSOURCES  = ${ALLSOURCE} ${OTHERS}
LISTING     = Listing.ps

HEADERS     = listmap.h xless.h xpair.h debug.h util.h
TCCFILES    = listmap.tcc util.tcc
CPPFILES    = debug.cpp main.cpp util.cpp

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $<

lint : ${CPPSOURCE}
	${UTILBIN}/cpplint.py.perl ${CPPSOURCE}

check : ${ALLSOURCES}
	${UTILBIN}/checksource ${ALLSOURCES}

ci : ${ALLSOURCES}
	${UTILBIN}/cid -is ${ALLSOURCES}

lis : ${ALLSOURCES}
	mkpspdf ${LISTING} ${ALLSOURCES}

clean :
	- rm ${OBJECTS} ${DEPFILE} core

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

dep : ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

submit :
	@ submit cse111-wm.f21 asg3 ${HEADERS} ${TCCFILES} ${CPPFILES} \
	Makefile README PARTNER

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

