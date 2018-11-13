// v0.1 board is 0.622" radius
// v0.2 board is 0.5642" radius (1 square inch)

// play between the links
link_play = 1.4;

// horizontal play between the teeth
teeth_play = 0.2;

band_height = 4;
pin_ratio = 3/5;


module link(w,d,h,w2, last=0, first=0)
{
	render() difference() {
		intersection() {
			translate([0,0,-h/2-1])
			linear_extrude(height=h+2)
			polygon([
				[-w2/2,-(d+h+(last?h:0))/2],
				[+w2/2,-(d+h+(last?h:0))/2],
				[+w/2,+(d+h)/2],
				[-w/2,+(d+h)/2],
			]);
		union() {

			cube([w2 > w ? w2 : w,d,h], center=true);

			// positive rounded edge A1, length w2
			translate([0,d/2,0])
			rotate([0,90,0]) 
			cylinder(d=h, h=w, $fn=32, center=true);

			// positive rounded edge B2
			translate([0,-d/2,0])
			rotate([0,90,0]) 
			cylinder(d=h, h=w2, $fn=32, center=true);

			// last one has a square clasp
			if(last)
			{
				translate([0,-h,0])
				cube([w/3-2*teeth_play,h,h], center=true);

				translate([0,-h*3/2,0])
				rotate([0,90,0]) 
				cylinder(d=h, h=w/3-2*teeth_play, $fn=32, center=true);
			}
		}
		}

		// negative large rounded edge B1
		translate([w2/3/2-teeth_play,-d/2,0])
		rotate([0,90,0]) 
		cylinder(d=h+link_play, h=w2/3+4, $fn=32);

		translate([-w2/3/2+teeth_play,-d/2,0])
		rotate([0,-90,0]) 
		cylinder(d=h+link_play, h=w2/3+4, $fn=32);

		// negative large rounded edge A2
		translate([0,d/2,0])
		rotate([0,90,0]) 
		cylinder(d=h+link_play, h=w2/3+2*teeth_play, $fn=32, center=true);

		// pin hole
		translate([0,-d/2,0])
		rotate([0,90,0]) 
		cylinder(d=h*pin_ratio, h=w+2, $fn=32, center=true);

		// if this is the last one,
		if(last) {
			// make the hole open on the bottom
			translate([0,-d/2,-1])
			//rotate([20,0,0])
			cube([w,h*pin_ratio,h/2+1], center=true);

			translate([0,-d/2-h/3,0])
			rotate([0,90,0]) 
			cylinder(d=h*pin_ratio, h=w+2, $fn=32, center=true);

			translate([0,-d/2-h/3/2,0])
			//rotate([20,0,0])
			cube([w,h*pin_ratio,h*pin_ratio], center=true);
		}

		if (first) {
			// and make the clasp side even large
			translate([0,+h/3,-1])
			cube([w2/3+2*teeth_play,h,h+2], center=true);
		}


		// maybe add some fun patterns
		if(0) {
		translate([w*1/6,d/2+h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		translate([w*3/6,d/2-h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		translate([w*5/6,d/2+h/4,-1]) cylinder(d=w/4, h=h+2, $fn=7);
		}
	}

	// pin
	translate([0,d/2,0])
	rotate([0,90,0])
	cylinder(d=h*pin_ratio-2*teeth_play, h=w-2, $fn=32, center=true);
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
		translate([+lug_offset+1,0,band_height/2])
		rotate([0,0,90])
		link(band_width, 8, band_height, w2=band_width-1);

		translate([-lug_offset-2,0,band_height/2])
		rotate([0,0,90])
		link(band_width, 8, band_height, w2=band_width);

		translate([-lug_offset,0,band_height/2])
		rotate([0,0,90])
		cube([band_width,5,band_height], center=true);

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
	band_width = band_width + band_count*0.5,
	height=6.5
);

if(1)
for(i=[1:band_count])
{
	translate([-i*band_length-14-2,0,band_height/2])
	rotate([0,0,90])
	link(
		band_width+(band_count-i+1)*0.5,
		band_length,
		band_height,
		band_width+(band_count-i)*0.5,
		first=(i==band_count)
	);
}

if(1)
for(i=[1:band_count-1])
{
	translate([+i*band_length+14+1,0,band_height/2])
	rotate([0,0,90])
	link(
		band_width + (band_count-i)*0.5,
		band_length,
		band_height,
		band_width + (band_count-i-1)*0.5,
		last=(i==band_count-1)
	);
}
} else {

// test band
for(i=[1:4])
{
	translate([+i*band_length+14+5,-band_width/2,0])
	rotate([0,0,90])
	link(band_width+i,band_length,band_height,band_width+i+1, last=(i==4), first=(i==1));
}
}

%translate([0,20,0]) cube([190,1,1], center=true);
