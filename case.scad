// v0.1 board is 0.622" radius
// v0.2 board is 0.5642" radius (1 square inch)

// play between the links
link_play = 1.4;

// horizontal play between the teeth
teeth_play = 0.2;

band_height = 4;


module link(w,d,h,last=0, first=0)
{
	render() difference() {
		union() {
			cube([w,d,h]);

			// positive rounded edge A1
			translate([0,d,h/2])
			rotate([0,90,0]) 
			cylinder(d=h, h=w, $fn=32);

			// positive rounded edge B2
			translate([0,0,h/2])
			rotate([0,90,0]) 
			cylinder(d=h, h=w, $fn=32);

			// last one has a square clasp
			if(last)
			{
				translate([w/3+teeth_play,-h/2+0*teeth_play,0])
				cube([w/3-2*teeth_play,h,h]);

				translate([w/3+teeth_play,-h/3,h/2]) rotate([0,90,0]) 
				cylinder(d=h, h=w/3, $fn=32);
			}
		}

		// negative large rounded edge B1
		translate([-1,0,h/2])
		rotate([0,90,0]) 
		cylinder(d=h+link_play, h=w/3+1+teeth_play, $fn=32);

		// negative large rounded edge B3
		translate([w*2/3-teeth_play,0,h/2])
		rotate([0,90,0]) 
		cylinder(d=h+link_play, h=w/3+1+teeth_play, $fn=32);

		// negative large rounded edge A2
		translate([w*1/3-teeth_play,d,h/2])
		rotate([0,90,0]) 
		cylinder(d=h+link_play, h=w/3+2*teeth_play, $fn=32);

		// pin hole
		translate([-1,0,h/2])
		rotate([0,90,0]) 
		cylinder(d=h/2, h=w+2, $fn=32);

		// if this is the last one,
		if(last) {
			// make the hole open on the bottom
			translate([0,-h/4+5*teeth_play,-1])
			rotate([20,0,0])
			cube([w,h/2,h/2+1]);
		}

		if (first) {
			// and make the clasp side even large
			translate([w/3-teeth_play,d-h,-1])
			cube([w/3+2*teeth_play,h,h+2]);
		}


		// maybe add some fun patterns
		if(0) {
		translate([w*1/6,d/2+h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		translate([w*3/6,d/2-h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		translate([w*5/6,d/2+h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		}
	}

	// pin
	translate([1,d,h/2])
	rotate([0,90,0])
	cylinder(d=h/2-2*teeth_play, h=w-2, $fn=32);
}

module case(dial,lug_offset,band_width,height)
{
render() difference()
{
	union() {
		intersection() {
		// 12-sided outside
		rotate([0,0,360/12/2]) cylinder(r=dial+3, h=height, $fn=12);
		// round off the corners slightly
		translate([0,0,-1]) cylinder(r=dial+3-0.1, h=height+2, $fn=180);
		// and round off the top slightly
		sphere(r=dial+3+0.7, $fn=60);
		}


		// lug
		translate([+lug_offset+5,-band_width/2,0])
		rotate([0,0,90])
		link(band_width, 8, band_height);

		translate([-lug_offset+3,-band_width/2,0])
		rotate([0,0,90])
		link(band_width, 8, band_height);

		// bump for button
		//translate([0,dial,height/4]) cylinder(r=3, h=height/2, $fn=30);
	}


	// tapered cylinder inside
	translate([0,0,height-1.5]) cylinder(r1=dial+0.4, r2=dial, h=1.6, $fn=60);

	// cutout for button
	translate([0,dial,height/4]) cylinder(r=3-0.1, h=height/2, $fn=30);

	// larger inside cylinder
	cylinder(r=dial+0.4, h=height-1.5, $fn=60);

/*
	// slot for strap
	translate([-14,0,0]) rotate([0,+30,0]) cube([20,band_width,2], center=true);
	translate([+14,0,0]) rotate([0,-30,0]) cube([20,band_width,2], center=true);
*/
}
}


// v0.1 case
if(0)
translate([0,25,0]) case(
	dial=0.622 * 25.4,
	lug_offset = 16,
	band_width = 22.2,
	height=8
);


// v0.2 case
band_width = 18;
band_length = 8;
band_count = round(160 / band_length / 2);

if(1)
{
case(
	dial= 0.5642 * 25.4,
	lug_offset = 14,
	band_width = band_width,
	height=6.5
);

for(i=[1:band_count])
{
	translate([-i*band_length-14+3,-band_width/2,0])
	rotate([0,0,90])
	link(band_width,band_length,band_height, first=(i==band_count));
}

for(i=[1:band_count-1])
{
	translate([+i*band_length+14+5,-band_width/2,0])
	rotate([0,0,90])
	link(band_width,band_length,band_height, last=(i==band_count-1));
}
} else {

// test band
for(i=[1:4])
{
	translate([+i*band_length+14+5,-band_width/2,0])
	rotate([0,0,90])
	link(band_width,band_length,band_height, last=(i==4), first=(i==1));
}
}

%translate([0,20,0]) cube([190,1,1], center=true);
