#!/usr/bin/env perl
use strict;

my $read_fd = @ARGV[0];
my $write_fd = @ARGV[1];

my $doc_start;
my $error_data;
my $line;
my @errors;

my $print_header;
my $header_was_printed;
my $reason_type;
my $r_type;
my $ori_r_type = "";
my $function_name = "";
my @wds_verbose_type = ("Mip", "Internal", "Cm", "3gpp", "Ppp", "Ehrpd", "Ipv6");

open(RH, '<', $read_fd) or die $!;
open(WF, '>', $write_fd) or die $!;

print WF <<EOF;
#define LOG(...) do { \\
	fprintf(stdout, ##__VA_ARGS__); fflush(stdout); \\
} while (0);

#define F_IN LOG("START[***\%s:\%d]\\n", __func__, __LINE__)
#define F_OUT LOG("END[***\%s:\%d]\\n\\n", __func__, __LINE__)
#define DD LOG("DD[***\%s:\%d]\\n", __func__, __LINE__)

struct Gen_ErrorEnumValue {
	int code;
	const char *text;
};


EOF

while ($line = <RH>) {
	chomp $line;

	$line =~ /^\/\*\*/ and do {
		$doc_start = 1;
		next;
	};

	$line =~ /^\s*\*\// and do {
		undef $error_data;
		if ( $header_was_printed == 1 ) {
			$function_name = 'qmi_wds_verbose_call_end_reason_' . $r_type . '_values';
			print WF "};\n\n";
			print WF "static char *qmi_wds_verbose_call_end_reason_" . $r_type . "_get_string(QmiWdsVerboseCallEndReason" . $ori_r_type . " val)\n";
print WF <<EOF;
{
	int i;
	char *error_str = NULL;

	for(i = 0; $function_name\[i].text; i++ ) {
		if (val == $function_name\[i].code) {
			error_str = calloc(1, strlen($function_name\[i].text) + 1);
			if (error_str) {
				strcpy(error_str, $function_name\[i].text);
				return error_str;
			}
			break;
		}
	}
	if (!error_str) {
		error_str = calloc(1, strlen("Unknown reason") + 1);
			if (error_str) {
				strcpy(error_str, "Unknown reason");
				return error_str;
			}
	}
	return error_str;
}


EOF
			$function_name = "";
			undef $header_was_printed;
		}
	};

	foreach $reason_type ( @wds_verbose_type ) {
		$doc_start and $line =~ /^\s*\*\s*QmiWdsVerboseCallEndReason$reason_type:/ and do {
			$r_type = lc($reason_type);
			$ori_r_type = "$reason_type";
			$error_data = 1;
			$print_header = 1;
			undef $doc_start;
			next;
		};
	}

	$line =~ /^\s*\*\s*@(.+): (.+)\./ and do {
		push @errors, [ $1, $2 ];
		undef $doc_start;

		if ( $error_data == 1 && $print_header == 1) {
			$header_was_printed = 1;
			print WF "struct Gen_ErrorEnumValue qmi_wds_verbose_call_end_reason_";
			print WF "$r_type";
			print WF "_values[] = {\n";
			print WF "\t{ ".$1.", \"".$2."\" },\n";
			undef $print_header
		} elsif ( $error_data == 1 ) {
			print WF "\t{ ".$1.", \"".$2."\" },\n";
		}
	};

}
close(RH);
close(WF);

