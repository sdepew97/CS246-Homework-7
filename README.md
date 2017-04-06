---
title: CS 246 Assignment 7
---

<div id="header">

| **CS 246 Assignment 7**
| due 12:55pm on Thursday, April 13, 2017

</div>

[Link back to syllabus](http://cs.brynmawr.edu/cs246/syllabus.html)

[Click here to start assignment](TODO)

Like all assignments, you will complete this assignment via
GitHub. See [the submission instructions](../submission.html)
for how to get the starter code and how to submit this
assignment.

You may want to use `valgrind` to test your work for memory safety. (Mac users
may want to install this with `brew install valgrind`.) If you do so, it will
be helpful to suppress memory errors caused by the `check` framework. To suppress
these errors, download the [check.supp](../14/check.supp) file and pass the
`--suppressions=check.supp` argument to `valgrind`. So, if your executable is
`check_dlist`, then you would call `valgrind --suppressions=check.supp ./check_dlist`
on the command line (with `check.supp` in the same folder as `check_dlist`).

You will likely also want to write a `Makefile` as part of this assignment.

