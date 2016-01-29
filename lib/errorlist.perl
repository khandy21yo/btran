#!/usr/bin/perl

$maxvalue = 0;

while(<>)
{
	chomp;
	if ($_ ne "")
	{
			$value = substr($_, 0, 9) * 1.0;
			$rest = substr($_, 10);
			($code, $text) = split(/,/, $rest, 2);
			$code =~ tr/A-Z/a-z/;

			print 'static long const bas$v_', $code, ' = ', $value, ";\n";
			$errdisc[$value] = $text;
			$errcode[$value] = $code;
			$maxvalue = $value if $value > $maxvalue;
	}
}

print "\n\n";

print "static const char* BasicErrorMessage[] =\n{\n";
foreach $value (0..$maxvalue)
{
	$text = "?" . substr($errdisc[$value], 1);
	$text = "?Unused" if $text eq "?";
	print "\t\"$text\",\t\\\\ $value\n";
}

print "};\n\n";

foreach $value (0..$maxvalue)
{
	$code = $errcode[$value];
	if ($code ne "")
	{
		print 'inline static long const bas$v_', $code, " = ", $value, ";\n";
	}
}

print "\n\n";
