#!/usr/bin/perl

use Config;
use ExtUtils::MakeMaker;

WriteMakefile(
    'NAME'	    => 'Win32::Symlink',
    'VERSION_FROM'  => 'Symlink.pm',
    'AUTHOR'        => 'Autrijus Tang <autrijus@autrijus.org>',
    'ABSTRACT'      => 'Symbolic links on Win32',
    (ExtUtils::MakeMaker->VERSION >= 6.21) ? (SIGN => 1) : ()
);

1;
