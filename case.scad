// board is 0.622" diameter

dial = 0.622 * 25.4;
height = 8;

render() difference()
{
	union() {
		// 12-sided outside
		rotate([0,0,360/12/2]) cylinder(r=dial+3, h=height, $fn=12);

		// lug
		translate([16,0,2]) cube([8,25,4], center=true);
		translate([-16,0,2]) cube([8,25,4], center=true);
	}

	// tapered cylinder inside
	translate([0,0,height-1.5]) cylinder(r1=dial+0.8, r2=dial, h=1.6, $fn=60);

	// larger inside cylinder
	cylinder(r=dial+0.8, h=height-1.5, $fn=60);

	// slot for strap
	translate([-14,0,0]) rotate([0,30,0]) cube([20,22.2,2], center=true);
	translate([+14,0,0]) rotate([0,-30,0]) cube([20,22.2,2], center=true);
}
