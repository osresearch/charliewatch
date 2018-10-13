// board is 0.622" diameter

dial = 0.622 * 25.4;
height = 8;

module lug()
{
translate([0,(dial+22)/2,4/2])
render() difference()
{
cube([22,9,4], center=true);
translate([0,2,0]) cube([20,1.5,10], center=true);
}
}

render() difference()
{
union() {
rotate([0,0,360/12/2])
cylinder(r=dial+3, h=height, $fn=12);

lug();
rotate([0,0,180]) lug();
}

// face
translate([0,0,height-2]) cylinder(r=dial+0.8, h=3, $fn=60);

// battery cr2405
//translate([0,0,height-1]) cube([26,15,2*height], center=true);
//translate([0,0,height-1]) cube([15,26,2*height], center=true);
translate([0,0,-1]) cylinder(d=30, h=10, $fn=60);

// holder
rotate([0,0,+30]) translate([0,0,height-2]) cube([32.5,8,4], center=true);

// lugs


}

