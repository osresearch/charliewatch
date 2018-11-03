// v0.1 board is 0.622" radius
// v0.2 board is 0.5642" radius (1 square inch)

module case(dial,lug_offset,band_width,height)
{
render() difference()
{
	union() {
		// 12-sided outside
		rotate([0,0,360/12/2]) cylinder(r=dial+3, h=height, $fn=12);

		// lug
		translate([+lug_offset,0,2]) cube([8,band_width+3,4], center=true);
		translate([-lug_offset,0,2]) cube([8,band_width+3,4], center=true);

		// bump for button
		//translate([0,dial,height/4]) cylinder(r=3, h=height/2, $fn=30);
	}


	// tapered cylinder inside
	translate([0,0,height-1.5]) cylinder(r1=dial+0.4, r2=dial, h=1.6, $fn=60);

	// cutout for button
	translate([0,dial,height/4]) cylinder(r=3-0.1, h=height/2, $fn=30);

	// larger inside cylinder
	cylinder(r=dial+0.4, h=height-1.5, $fn=60);

	// slot for strap
	translate([-14,0,0]) rotate([0,+30,0]) cube([20,band_width,2], center=true);
	translate([+14,0,0]) rotate([0,-30,0]) cube([20,band_width,2], center=true);
}
}


// v0.1 case
%translate([0,25,0]) case(
	dial=0.622 * 25.4,
	lug_offset = 16,
	band_width = 22.2,
	height=8
);

// v0.2 case
case(
	dial= 0.5642 * 25.4,
	lug_offset = 14,
	band_width = 18,
	height=6.5
);

