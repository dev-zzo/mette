#!/usr/bin/env perl

use strict;
use warnings;

my @decls = ();
my @inits = ();

sub make_hash {
	my $id = shift;
	my $hash = 0;

	foreach (split //, $id) {
		$hash = (($hash * 61 + ord $_) ^ ($hash >> 16)) & 0xFFFFFFFF;
	}

	return sprintf "0x%08XU", $hash;
}

sub process_thunks {
	my $stream = shift;
	while (<$stream>) {
		if (/VM_THUNK\s*[(]([_a-zA-Z0-9]+)\s*[)]/) {
			my $id = $1;
			my $hash = make_hash($id);
			push @decls, "void __thunkproc_$id(vm_context_t *ctx);\n";
			push @inits, "\t{ $hash, __thunkproc_$id },\n";
		}
	}
}

foreach my $fname (@ARGV) {
	open my $fh, $fname or die "Couldn't read $fname: $!";
	process_thunks($fh);
	close $fh;
}

foreach (@decls) {
	print;
}

print "\nconst vm_thunk_record_t vm_thunks[] __attribute__((section(\".vm.thunks\"))) = {\n";
foreach (@inits) {
	print;
}
print "};\n\n";
