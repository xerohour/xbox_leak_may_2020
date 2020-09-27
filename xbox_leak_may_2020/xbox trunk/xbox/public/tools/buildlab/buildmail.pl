#build bvtresults status page
use Getopt::Long;
use File::Copy;

GetOptions("b=s"  => \$build,
           "h=s"  => \$headline,
           "p"    => \$pvt,
           "c"    => \$cp);

if(!$build) {die "Usage:  perl bvtresults {-b build_num} [-h headline] [-p] [-c]";}
if($pvt){ $release = "pvtrelease"} else {$release = "release"}

open(TABLE, "checkins.csv");
open(ONLINE, "onlinecheckins.csv");
open(BVTR,  ">checkins.htm");
open(TOP,   "bvtres-top.htm");
open(MID1,   "bvtres-mid1.htm");
open(MID,   "bvtres-mid.htm");
open(TAIL,  "bvtres-end.htm");

while(<TOP>){
    s/<!--BUILDNUM-->/$build/g;
    s/<!--Headline-->/$headline/;
    print BVTR;
}
while(<TABLE>){
    if(!m/"(.*)","Subject"/) {
        s/"(.*)","(.*)"/<tr><td>\1<\/td><td>\2<\/td><\/tr>/;
        print BVTR;
    }
}
print BVTR <MID1>;
while(<ONLINE>){
    if(!m/"(.*)","Subject"/) {
        s/"(.*)","(.*)"/<tr><td>\1<\/td><td>\2<\/td><\/tr>/;
        print BVTR;
    }
}
print BVTR <MID>;
system "grep \"FAIL,\" \\\\xbuilds\\$release\\usa\\$build\\checked\\results\\*.log > fail.txt";
open(FAIL,  "fail.txt");
print BVTR <FAIL>;
print BVTR <TAIL>;
if($cp){
    #copy(checkins.htm, \\\\xbox\\bvt\\bvtresults.htm) or die "copy failed: $!";
}
