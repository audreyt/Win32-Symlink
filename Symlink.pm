package Win32::Symlink;

use strict;
use vars qw($VERSION @ISA);
use DynaLoader;

@ISA = qw(DynaLoader);
$VERSION = '0.02';

__PACKAGE__->bootstrap($VERSION);

sub import {
    *CORE::GLOBAL::symlink = __PACKAGE__->can('symlink');
}

1;

__END__

=head1 NAME

Win32::Symlink - Symlink support on Windows

=head1 VERSION

This document describes version 0.02 of Win32::Symlink, released
September 19, 2004.

=head1 SYNOPSIS

    use Win32::Symlink;
    symlink( 'from' => 'to' );

=head1 DESCRIPTION

This module implements the built-in C<symlink> function for Microsoft Windows.
Currently, it only works for NTFS filesystems.

=head1 SEE ALSO

L<Win32::Hardlink>

=head1 AUTHORS

Autrijus Tang E<lt>autrijus@autrijus.orgE<gt>

=head1 COPYRIGHT

Copyright 2004 by Autrijus Tang E<lt>autrijus@autrijus.orgE<gt>.

This program is free software; you can redistribute it and/or modify it
under the same terms as Perl itself.

See L<http://www.perl.com/perl/misc/Artistic.html>

=cut
