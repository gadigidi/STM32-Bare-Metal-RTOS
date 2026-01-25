#!/usr/bin/perl

my $base_addr = 0x2000448;
foreach my $i(0.. 32){
    my $addr = $base_addr - $i*4;
    printf ("0x%x\n", $addr);
}



