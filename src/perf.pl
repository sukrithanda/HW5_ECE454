#!/usr/bin/perl
#
# Script for ECE454 Lab5

use warnings;

my $root;
chomp ($root = `pwd`);
my $make_cmd = "make clean; make";
my $run_cmd = "/usr/bin/time -f \"%e real\" ./gol 10000 inputs/1k.pbm outputs/1k.pbm";

my @versions = qw(old_src mod_src);

my $mod_result = 200;
my $old_result = 200;

print "=====================================\n";

foreach my $ver (@versions) {
        #print "Changing dir - $root/$ver\n";
        chdir("$root/$ver");

	if ($ver eq "old_src") {
		print "Running Original Code";
	} 

	if ($ver eq "mod_src") {
                print "Running Modified Code";
        }

       
	if ($ver eq "mod_src") { 
		my @output = `$make_cmd 2>&1 1>/dev/null`;
	        foreach my $line (@output) {
	        	if ($line =~ /error/) {
	                	die("Make Failed.\n");
	       		}
	        }
	}

	if ($ver eq "old_src") {
		#$old_result = `$run_cmd 2>&1 1>/dev/null`;
		#$old_result =~ s/real//g;
		#chomp ($old_result);
		print " - $old_result\n";
	}

        if ($ver eq "mod_src") {
                $mod_result = `$run_cmd 2>&1 1>/dev/null`;
		$mod_result =~ s/real//g;
		chomp ($mod_result);
		print " - $mod_result\n";
        }

}

my $impr = $old_result/$mod_result;
$impr = sprintf "%.2f", $impr;
print "Improvement - $impr\n";

chdir($root);
my @diffResult = `diff $versions[0]/outputs/1k_verify_out.pbm $versions[1]/outputs/1k.pbm`;
if (@diffResult) {
	print "Sanity failed\n@diffResult\n";
} else {
	print "Sanity passed\n";
}

print "=====================================\n";

exit;
