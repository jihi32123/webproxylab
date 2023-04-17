#! /usr/bin/perl -w
use strict;
use Digest::MD5;
#
# port-for-user.pl - Return a port number, p, for a given user, with a
#     low probability of collisions. The port p is always even, so that
#     users can use p and p+1 for testing with proxy and the Tiny web
#     server.
#     
#     usage: ./port-for-user.pl [optional user name]
#
my $maxport = 65536;
my $minport = 1024;


# hashname - compute an even port number from a hash of the argument
sub hashname {
    my $name = shift;
    my $port;
    my $hash = Digest::MD5::md5_hex($name);
    # take only the last 32 bits => last 8 hex digits
    $hash = substr($hash, -8);
    $hash = hex($hash);
    $port = $hash % ($maxport - $minport) + $minport;
    $port = $port & 0xfffffffe;
    print "$name: $port\n";
}


# If called with no command line arg, then hash the userid, otherwise
# hash the command line argument(s).
if($#ARGV == -1) {
    my ($username) = getpwuid($<);
    hashname($username);
} else {
    foreach(@ARGV) {
        hashname($_);
    }
}

# 해당 코드는 Perl 스크립트로, 주어진 사용자 이름 또는 명령행 인자에 대한 MD5 해시를 계산하고 해당 해시의 마지막 8자리를 포트 번호로 변환하는 역할을 합니다.

# 보다 구체적으로, 이 스크립트는 사용자 이름 또는 명령행 인자를 입력받습니다. 
# 사용자 이름이 입력되지 않은 경우, 스크립트는 현재 실행중인 사용자의 이름을 가져와서 해당 사용자 이름에 대한 MD5 해시를 계산합니다. 
# 그렇지 않은 경우, 입력된 모든 명령행 인자에 대해 MD5 해시를 계산합니다.

# MD5 해시는 32자리 16진수 숫자로 이루어져 있으며, 이 스크립트는 이 해시의 마지막 8자리를 포트 번호로 변환합니다.
#  변환된 포트 번호는 1024부터 65536까지의 범위에서 계산되며, 항상 짝수로 설정됩니다. 
# 이는 Tiny 웹 서버 및 프록시와 같은 다른 프로그램에서 두 개의 연속된 포트 번호를 사용할 수 있도록 하기 위함입니다. 
# 마지막으로, 각 입력된 사용자 이름 또는 명령행 인자와 해당 포트 번호가 출력됩니다.
