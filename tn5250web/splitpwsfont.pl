#!/usr/bin/perl -w

use strict;

while (<>) {
  if (/^%!PS-AdobeFont-1\.0: ([\w-]+)/) {
    open OUTPUT, "> $1.pfa" || die;
  } elsif (/^%!PS-Adobe-2\.0/) {
    open OUTPUT, "> pwsubs.ps" || die;
  }

  print OUTPUT $_;
}
