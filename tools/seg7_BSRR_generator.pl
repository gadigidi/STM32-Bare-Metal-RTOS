#!/usr/bin/perl

my @segments = qw(A B C D E F G DEC);

my @nums = ();

push @nums, [qw(A B C D E F)]; #0
push @nums, [qw(B C)]; #1
push @nums, [qw(A B D E G)]; #2
push @nums, [qw(A B C D G)]; #3
push @nums, [qw(F G B C)]; #4
push @nums, [qw(A F G C D)]; #5
push @nums, [qw(A F E D C G)]; #6
push @nums, [qw(A B C)]; #7
push @nums, [qw(A B C D E F G)]; #8
push @nums, [qw(A B C D F G)]; #9
push @nums, [qw(A B C E F G)]; #A
push @nums, [qw(C D E F G)]; #b
push @nums, [qw(A D E F)]; #C
push @nums, [qw(B C D E G)]; #d
push @nums, [qw(A D E F G)]; #E
push @nums, [qw(A E F G)]; #F
push @nums, [qw()]; #NONE
push @nums, [qw(A)]; #Top horizontal
push @nums, [qw(G)]; #Middle horizontal
push @nums, [qw(D)]; #Bottom horizontal
push @nums, [qw(B)]; #Top right
push @nums, [qw(C)]; #bottom right
push @nums, [qw(F)]; #Top left
push @nums, [qw(E)]; #Bottom left
push @nums, [qw(C E G)]; #r
push @nums, [qw(B C D E F)]; #U
push @nums, [qw(A C D E F G)]; #G

my $len = @nums;

my %seg_ports = (
                 "A" => 5,
                 "B" => 6,
                 "C" => 7,
                 "D" => 8,
                 "E" => 9,
                 "F" => 10,
                 "G" => 13,
                 "DEC" => 14,
               );

my %dig_ports = (
                 "1" => 0,
                 "2" => 1,
                 "3" => 2,
                 "4" => 3,
               );
           
print "const uint32_t seg_bsrr[$len] = {\n";
my $i = 0;
foreach my $num(@nums){
  my $mask = 0;
  foreach my $seg(@segments){

    if (grep { $_ eq $seg } @$num) {
      #print "$seg found at num $i\n";
      $mask |= (1<<$seg_ports{"$seg"});
    }
    else{
      $mask |= (1<<($seg_ports{"$seg"}+16));
      #print "$seg not found at num $i\n";
    }
  }
  printf "    ${mask}U";
  if ($i<($len-1)){
    print ",\n";
  }
  else{
    print "\n";
  }
  $i++;
}  
print "};\n\n";



print "const uint32_t dig_bsrr[4] = {\n";
foreach my $digit (1.. 4){
  my $mask = 0;
  foreach my $i (0.. 3){
    if ($digit == ($i+1)) {
      $mask |= (1<<($dig_ports{"$digit"}+16));
      #my $temp = $dig_ports{"$digit"}+16;
      #print "debug: bit $temp is set to turn on digit $digit\n";
    }
    else{
      my $dig = $i+1;
      $mask |= (1<<$dig_ports{"$dig"});
      #my $temp = $dig_ports{"$dig"};
      #print "debug: bit $temp is set to turn off digit $dig\n";
    }
  }
  print "    ${mask}U";
  if ($digit<4){
    print ",\n";
  }
  else{
    print "\n";
  }
}
print "};\n\n";

my $mask = 0;
foreach my $i (0.. 3){
  my $digit = $i+1;
  $mask |= (1<<$dig_ports{"$digit"});
}
print "#define SEG7_BSRR_DIGS_OFF\t\t${mask}U\n\n";
