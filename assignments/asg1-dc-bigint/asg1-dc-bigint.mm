.so Tmac.mm-etc
.if t .Newcentury-fonts
.INITR* \n[.F]
.SIZE 12 14
.ds Quarter Fall\~2021
.TITLE CSE-111 \*[Quarter] Program\~1 "Overloading and operators"
.RCS "$Id: asg1-dc-bigint.mm,v 1.233 2021-10-11 16:03:11-07 - - $"
.PWD
.URL
.if t .nr VCODENWIDTH \w'\f[CB]\0\0\0\fR'
.if n .nr VCODENWIDTH \w'\0\0\0'
.nr SHELLCNT 0 1
.de COMMAND
.   br
.   ft CR
.   nop -bash-\\n+[SHELLCNT]$
.   ft CB
.   nop \\$*
.   ft CR
.   br
.   nf
.   tm \\$*
.   pso \\$* | expand
.   br
.   ft R
.   fi
..
.H 1 "Using C++11/14/17 (\f[CB]g++ -std=gnu++2a\f[P])"
All programming in this course will be done C++ style,
not C style.
.DS
.TS
center tab(|); |le |le |.
_
Do not use\(::|Instead, use\(::
_
\f[CB]char*\fP strings|\f[CB]<string>\fP
C arrays|\f[CB]<vector>\fP
T{
\f[CB]<stdio.h>\fP, \f[CB]<cstdio>\fP
T}|T{
\f[CB]<iostream>\fP, \f[CB]<iomanip>\fP
T}
pointers|\f[CB]<shared_ptr>\fP
\f[CB]union\fP|inheritance
\f[CB]<\fP\fIheader\fP\f[CB].h>\fP|\f[CB]<c\fP\fIheader\fP\f[CB]>\fP
_
.TE
.DE
Include only C++ header files and use the declaration
.V= "using namespace std;"
Include
.VI <c header >
files only when C++ header files do not provide a necessary facility.
Include
.VI < header \&.h>
files from C only when an appropriate
.VI <c header >
file does not exist.
Use the script
.V= cpplint.py.perl
(a wrapper for
.V= cpplint.py )
to check style.
.P
The production system for all work is
.V= unix.ucsc.edu 
using
.V= g++ .
Compile with
.ds G++OPTS -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
.VTCODE* 1 "g++ -std=gnu++2a -g -O0 \*[G++OPTS]"
Following is a description of these options\(::
.ALX-BL
.LI
.V= -std=gnu++2a
specifies the experimental Gnu dialect of C++20,
still experimental,
beyond the current C++17 standard.
.LI
.V= -g
produces debugging information into object files and the binary
executable.
This is necessary for
.V= gdb
and
.V= valgrind
to use symbolic names.
.LI
.V= -O0
reduces compilation time and makes debugging produce more expected
results.
Optimization may rearrange bugs in code in unexpected ways.
.LI
.V= -Wall
enables all the warnngs about questionable constructions.
.LI
.V= -Wextra
enables extra warnings that are not enabled with
.V= -Wall .
.LI
.V= -Wpedantic
issues all warnings required by strict ISO C++ and rejects
all programs that do not conform to ISO C++.
.LI
.V= -Wshadow
warns whenever a local variable or declaration
shadows another variable, parameter, or class member.
.LI
.V= -Wold-style-cast
warns about the use of any old-style (C-style) cast.
Instead,
use
.V= static_cast ,
.V= dynamic_cast ,
.V= const_cast ,
or
.V= reinterpret_cast  .
.LI
.V= -fdiagnostics-color=never 
prevents the compiler from using those silly annoying colors
in diagnostics.
.LE
.P
The particular
.V= g++
compiler we will be using is
.in +\n[VCODENWIDTH]u
.COMMAND which g++
.COMMAND g++ --version | grep -i g++
.COMMAND uname -npo
.in -\n[VCODENWIDTH]u
.P
If you develop on your personal system,
be sure to port and test your code on the Linux timeshares.
If it compiles and runs on your system, but not on the timeshares,
.ft BI
then it does not work.
.ft P
.H 1 "Overview"
This assignment will involve overloading basic integer operators
to perform arbitrary precision integer arithmetic in the style of
.V= dc (1).
Your class
.V= bigint
will intermix arbitrarily with simple integer arithmetic.
.P
To begin read the
.V= man (1)
page for the command
.V= dc (1)\(::
.VCODE* 1 "man -s 1 dc"
A copy of that page is also in this directory.
Your program will use the standard
.V= dc
as a reference  implemention and must produce exactly
the same output for the commands you have to implement\(::
.VCODE* 1 "+\0 -\0 *\0 /\0 %\0 \[ha]\0 c\0 d\0 f\0 p\0 q\0"
.P
If you have any questions as to the exact format of your output,
just run
.V= dc (1)
and make sure that, for the operators specified above,
your program produces exactly the same output.
A useful program to compare output from your program with that of
.V= dc (1)
is
.V= diff (1),
which compares the contents of two files and prints only the
differences.
Also look in the subdirectory
.V= misc/
for some examples.
.P
See also\(::
.ALX-BL
.LI
dc (computer program)
.br
.V= https://en.wikipedia.org/wiki/Dc_(computer_program)
.LI
dc, an arbitrary precision calculator
.br
.V= https://www.gnu.org/software/bc/manual/dc-1.05/html_mono/dc.html
.LE
.H 1 "Implementation strategy"
As before, you have been given starter code.
.ALX a ()
.LI
.V= Makefile ,
.V= debug ,
and
.V= util
If you find you need a function which does not properly belong to
a given module, you may add it to
.V= util .
.LI
The module
.V= scanner
reads in tokens, namely a
.V= NUMBER ,
an
.V= OPERATOR ,
or
.V= SCANEOF .
Each token returns a
.V= token_t ,
which indicates what kind of token it is
(the
.V= "terminal_symbol symbol" ),
and the
.V= "string lexinfo"
associated
with the token.
Only in the case of a number is there more than one character.
Note that on input, an underscore
.=V ( _ )
indicates a negative number.
The minus sign
.=V ( - )
is reserved only as a binary operator.
The scanner also has defined a couple of
.V= operator<<
for printing out scanner results in debug mode.
.LI
The main program
.V= main.cpp ,
has been implemented for you.
For the six binary arithmetic functions,
the right operand is popped from the stack,
then the left operand,
then the result is pushed onto the stack.
.LI
The module
.V= iterstack
can not just be the STL
.V= stack ,
since we want to iterate from top to bottom,
and the STL
.V= stack
does not have an iterator.
A stack depends on the operations
.V= back() ,
.V= push_back() ,
and
.V= pop_back()
in the underlying container.
We could use a
.V= vector ,
a
.V= deque ,
or just a
.V= list ,
as long as the requisite operations are available.
.LE
.H 1 "Class \f[CB]bigint\f[P]"
Then we come to the most complex part of the assignment,
namely the class
.V= bigint .
Operators in this class are heavily overloaded.
.ALX a ()
.LI
Most of the functions take a arguments of type
.V= "const bigint&" ,
i.e.,
a constant reference,
for the sake of efficiency.
But they have to return the result by value.
.LI
The
.V= operator<<
can't be a member since its left operand is an
.V= ostream ,
so we make it a
.V= friend ,
so that it can see the innards of a
.V= bigint .
Note now
.V= dc
prints really big numbers.
.LI
The relational operators
.V= ==
and
.V= <
are coded individually as member functions.
The others,
.V= != ,
.V= <= ,
.V= > ,
and
.V= >=
are defined in terms of the essential two.
.LI
All of the functions of
.V= bigint
only work with the sign,
using
.V= ubigint
to do the actual computations.
So
.V= bigint::operator+
and
.V= bigint::operator-
will check the signs of the two operands then call
.V= ubigint::operator+
or
.V= ubigint::operator- ,
as appropriate,
depending on the relative signs and magnitudes.
The multiplication and division operators just call the corresponding
.V= ubigint
operators,
then adjust the resulting sign according to the rule of signs.
.H 1 "Class \f[CB]ubigint\f[P]"
Class
.V= ubigint
implements unsigned large integers and is where the computational
work takes place.
Class
.V= bigint
takes care of the sign.
Now we turn to the representation of a
.V= ubigint ,
which will be represented by vector of bytes.
.ALX a ()
.LI
Replace the declaration
.VCODE* 1 "using unumber = unsigned long;"
.VCODE* 1 "unumber uvalue {};
with
.VCODE* 1 "using ubigvalue_t = vector<uint8_t>;"
.VCODE* 1 "ubigvalue_t ubig_value;"
in the header file
.V= <ubigint.h> .
The type
.V= uint8_t
is an unsigned 8-bit integer defined in
.V= <cstdint> .
.LI
In storing the big integer, each digit is kept as an integer in
the range 0 to 9 in an element of the vector.
Since the arithmetic operators add and subtract work from least
significant digit to most significant digit,
store the elements of the vector in the same order.
That means, for example, that the number 4629 would be stored in
a vector
.V= v
as\(::
.V= v[3]==4 ,
.V= v[2]==6 ,
.V= v[1]==2 ,
.V= v[0]==9 .
In other words,
if a digit's place value is
.V= "d*pow(10,k)"
then
.V= v[k]==d .
.LI
In order for the comparisons to work correctly,
always store numbers in a canonical form\(::
After computing a value from any one of the six arithmetic operators,
always trim the vector by removing all high-order zeros\(::
.VTCODE* 1 "while (size() > 0 and back() == 0) pop_back();"
.LI
Canonical form.
.ALX-ITEMS
.LI
Zero is represented as a vector of size zero
and a positive sign.
.LI
All digits are stored as
.V= uint8_t
values in the range 0...9,
not as characters in the range
.V= '0' ... '9' .
.LI
To print a digit, cast it to an integer\(::
.V= static_cast<int>\~(digit) .
.LE
.LI
The scanner will produce numbers as
.V= string s,
so scan each string from the end of the string,
using a
.V= const_reverse_iterator
(or other means)
from the end of the string (least significant digit)
to the beginning of the string (most significant digit)
using
.V= push_back
to append them to the vector.
.LE
.H 1 "Implementation of operators"
.ALX a ()
.LI
For
.V= bigint::operator+ ,
check the signs.
.ALX 1 ()
.LI
If the signs are the same\(::
.ALX-ITEMS
.LI
Call
.V= ubigint::operator+ 
with the unsigned numbers.
.LI
The sign of the result is the sign of either number.
.LE
.LI
If the signs are different\(::
.ALX-ITEMS
.LI
Call
.V= ubigint::operator- 
with the larger number as its left number.
.LI
The sign of the result is the sign of the larger number.
.LE
.LE
.LI
The operator
.V= bigint::operator- ,
check the signs.
.ALX 1 ()
.LI
If the signs are different\(::
.ALX-ITEMS
.LI
Call
.V= ubigint::operator+
with the unsigned numbers.
.LI
The sign of the result is the sign of the left number.
.LE
.LI
If the signs are the same\(::
.ALX-ITEMS
.LI
Call
.V= ubigint::operator- 
with the larger number as its left number.
.LI
If the left number is larger,
the sign of the result is its sign.
.LI
Else
the the result has the opposite of the sign of the right number.
.LE
.LE
.LI
To implement
.V= ubigint::operator+ ,
create a new
.V= ubigint
and proceed from the low order end to the high order end,
adding digits pairwise.
For any sum >= 10,
take the remainder and add the carry to the next digit.
Use
.V= push_back
to append the new digits to the
.V= ubigint .
When you run out of digits in the shorter number,
continue, matching the longer vector with zeros,
until it is done.
Make sure the sign of 0 is positive.
.LI
To implement
.V= ubigint::operator- ,
also create a new empty vector, starting from the low order end
and continuing until the high end.
If the left digit is smaller than the right digit,
the subtraction will be less than zero.
In that case, add 10 to the digit,
and set the borrow to the next digit to \-1.
After the algorithm is done,
.V= pop_back
all high order zeros from the vector before returning it.
Make sure the sign of 0 is positive.
.LI
To implement
.V= operator== ,
check to see if the signs are the same and the lengths of
the vectors are the same.
If not, return false.
Otherwise run down both vectors and return false as soon a difference
is found.
Otherwise return true.
.LI
To implement
.V= operator< ,
remember that a negative number is less than a positive number.
If the signs are the same,
for positive numbers, the shorter one is less,
and for negative nubmers, the longer one is less.
If the signs and lengths are the same,
run down the parallel vectors from the high order end to the low
order end.
When a difference is found, return true or false, as appropriate.
If no difference is found, return false.
.LI
Implement function
.V= bigint::operator* ,
which uses the rule of signs to determine the result.
The number crunching is delegated to
.V= ubigint::operator* ,
which produces the unsigned result.
.LI
Multiplication in
.V= ubigint::operator*
proceeds by allocating a new vector
whose size is
equal to the sum of the sizes of the other two operands.
If
.V= u
is a vector of size
.V= m
and
.V= v
is a vector of size
.V= n ,
then in
.V= O(mn)
speed,
perform an outer loop over one argument and an inner loop over
the other argument, adding the new partial products to the product
.V= p
as you would by hand.
The algorithm can be described as follows\(::
.DS
.VCODE* 1 "p = all zeros"
.VCODE* 1 "for i in interval [0,m):"
.VCODE* 2     "carry = 0"
.VCODE* 2     "for j in interval [0,n):"
.VCODE* 3         "digit = p[i+j] + u[i] * v[j] + carry"
.VCODE* 3         "p[i+j] = digit % 10"
.VCODE* 3         "carry = digit / 10"
.VCODE* 2     "p[i+n] = carry"
.DE
Note that the interval
.V= [a,b)
refers to the half-open interval including
.V= a
but excluding
.V= b .
This is the set
.V= "{x| a<=x && x<b}" .
In the same way,
a pair of iterators in C++ is used to bound an interval
(begin and end pair).
.LI
Long division is complicated if done correctly.
See a paper by P. Brinch Hansen,
``Multiple-length division revisited\(::
A tour of the minefield'',
.I "Software \[em] Practice and Experience 24",
(June 1994), 579\[en]601.
Algorithms 1 to 12 are on pages 13\[en]23,
Note that in Pascal,
array bounds are part of the type,
which is not true for
.V= vector s
in C++.
.ALX-ITEMS
.LI
.V= multiple-length-division.pdf
.LI
.V= http://brinch-hansen.net/papers/1994b.pdf
.LI
.V= http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.14.5815
.LE
.LI
The function
.V= divide
as implemented uses the ancient Egyptian division algorithm,
which is slower than Hansen's Pascal program,
but is easier to understand.
Replace the
.V= long
values in it by
.V= vector<digit_t> .
The logic is shown also in
.V= misc/divisioncpp.cpp .
The algorithm is rather slow,
but the big-O analysis is reasonable.
.LI
The unsigned division function that is provided
depends on two private functions,
.V= multiply_by_2
and
.V= divide_by_2 ,
which are in-lace non-constant functions.
They both perform
.V= without
creating a new object.
.ALX 1 ()
.LI
To implement
.V= multiply_by_2 ,
iterate from the low order digit,
and double each digit (remainder 10),
carrying to the next higher digit.
At the end, if the carry is 1, use
.V= push_back.
.LI
To implement
.V= divide_by_2 ,
iterate from the low order digit,
and divide each digit by 2.
Then,
if the next higher digit is odd,
add 5 to the current digit.
Be careful of the end,
and
.V= pop_back
any remaining high order zeros.
.LE
.LI
Modify
.V= operator<< ,
first just to print out the number all in one line.
You will need this to debug your program.
When you are finished,
make it print numbers in the same way as
.V= dc (1)
does.
.LI
The
.V= pow
function uses other operations to raise
a number to a power.
If the exponent does not fit into a single
.V= long
print an error message,
otherwise do the computation.
The power function is not a member of either
.V= bigint
or
.V= ubigint ,
and is just considered a library function that is implemented
using more primitive operations.
.LE
.H 1 "Memory leak and other problems"
Make sure that you test your program completely so that it does not
crash on a Segmentation Fault or any other unexpected error.
Since you are not using pointers,
and all values are inline,
there should be no memory leak.
Use
.V= valgrind (1)
to check for and eliminate uninitialized variables and memory leak.
.P
If your program is producing strange output or segmentation faults,
use
.V= gdb (1)
and the debug macros in the
.V= debug
module of the
.V= code/
subdirectory.
.H 1 "What to submit"
Submit source files and only source files\(::
.V= Makefile ,
.V= README ,
and all of the header and implementation files necessary to build
the target executable.
If
.V= gmake
does not build
.V= ydc
your program can not be tested and you lose 1/2 of the points for
the assignment.
Use
.V= checksource
on  your code to verify basic formatting.
.P
Look in the grader's score
subdirectory for instructions to graders.
Read
.V= Syllabus/pair-programming/
and also submit
.V= PARTNER 
if you are doing pair programming.
Either way submit the
.V= README
described therein.
.char-nt he' he \[*e]\h'-3p'\*[+<]\h'3p'\*[+']
.ds kai-ta-hetera \
\[*k]\[*a]\[*i`] \[*t]\[*a`] \|\[he']\[*t]\[*e]\[*r]\[*a]
.if n .H 1 "Et cetera."
.if t .H 1 "Et cetera \~(\*[kai-ta-hetera])."
The accuracy of the Unix utility
.V= dc (1)
can be checked by\(::
.VTCODE* 0 "echo '82 43/25 43+65P80P82P73P76P32P70P79P79P76P10P' | dc"
