#!/usr/bin/perl
# convert the new gcvs3 on stdin to .edb format on stdout. 
# (c) 1998-2000 Elwood Charles Downey
# V 0.1 27 Oct 97 first cut
# V 0.2 19 May 00 different format.
# V 0.3 22 Jul 00 gcvs3 format by Fridger Schrempp, fridger.schrempp@desy.de
# Results are partly equinox 2000.0, partly 1950.0 data.

# boilerplate
($ver = '$Revision: 1.1.1.1 $') =~ s/\$//g;
($me = $0) =~ s/.*\///;
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime;
$year += 1900;
$mon += 1;
print "# GCVS3: ftp://ftp.sai.msu.su/pub/groups/cluster/gcvs/gcvs/iii/\n";
print "# File dated Apr 29 2000\n";
print "# Magnitude is set to the maximum magnitude of the object.\n";
print "# Generated by $me $ver\n";
print "# Processed $year-$mon-$mday $hour:$min:$sec UTC\n";

# map names to xephem's greek method
%greek = (
    'alf' => 'Alpha',  'bet' => 'Beta',  'del' => 'Delta', 'eps' => 'Epsilon',
    'eta' => 'Eta',    'gam' => 'Gamma', 'iot' => 'Iota',  'kap' => 'Kappa',
    'lam' => 'Lambda', 'mu ' => 'Nu',    'nu ' => 'Nu',    'ome' => 'Omega',
    'omi' => 'Omicron','phi' => 'Phi',   'pi ' => 'Pi',    'psi' => 'Psi',
    'rho' => 'Rho',    'sig' => 'Sigma', 'tau' => 'Tau',   'ups' => 'Upsilon',
    'zet' => 'Zeta'
);

while (<>) {
    chop();

    $name = &s (9, 18);
    $name =~ s/ +/ /g;
    $name =~ s/ *$//;
    $name =~ s/^ *//;
    if ($g = $greek{substr($name,0,3)}) {
	$name = sprintf "%s %s", substr($name,4), $g;
    }
    if (&s (38, 52) =~ /\d+/){
	$equinox = "2000";
	$rah = &s (38, 39); $rah =~ s/ //g;
	$ram = &s (40, 41); $ram =~ s/ //g;
	$ras = &s (42, 45); $ras =~ s/ //g;

	$dsign = &s (46, 46); $dsign =~ s/\+//;
	$decd  = &s (47, 48); $decd  =~ s/ //g;
	$decm  = &s (49, 50); $decm  =~ s/ //g;
	$decs  = &s (51, 52); $decs  =~ s/ //g;
    } else {
	$equinox = "1950";
	$rah = &s (21, 22); $rah =~ s/ //g;
	$ram = &s (23, 24); $ram =~ s/ //g;
	$ras = &s (25, 28); $ras =~ s/ //g;

	$dsign = &s (29, 29); $dsign =~ s/\+//;
	$decd  = &s (30, 31); $decd  =~ s/ //g;
	$decm  = &s (32, 33); $decm  =~ s/ //g;
	$decs  = &s (34, 35); $decs  =~ s/ //g;
    }

    $maxmag = &s (66, 71);  $maxmag =~ s/ //g;
    $spect  = &s (135,136); $spect  =~ s/ //g;
    $spect =~ s/-//g;
    if ($spect =~ /^[A-Z]/) {
	$spect = "|" . $spect;
    } else {
	$spect = "";
    }

    # beware of missing fields
    next if ($rah eq "" || $decd eq "" || $maxmag eq "");

    $type = &s (54, 63); $type =~ s/ //g;
    if ($type =~ /^SN/) {
	$type = "R";	# supernova remnant
    } elsif ($type =~ /^[EX]/) {
	$type = "B";	# eclipsing binary or xray pair
    } elsif ($type =~ /BLLAC/ || $type =~ /GAL/ || $type =~ /QSO/) {
	$type = "Q";	# QSO
    } else {
	$type = "V";	# other variable
    }

    printf "%s,f|%s%s,%s:%s:%s,%s%s:%s:%s,%s,%s\n", $name, $type, $spect,
	    $rah, $ram, $ras, $dsign, $decd, $decm, $decs, $maxmag,$equinox; 
}

# like substr($_,first,last), but one-based.
sub s {
    substr ($_, $_[0]-1, $_[1]-$_[0]+1);
}

# For RCS Only -- Do Not Edit
# @(#) $RCSfile: gcvs2edb.pl,v $ $Date: 2001/04/19 21:12:06 $ $Revision: 1.1.1.1 $ $Name:  $
