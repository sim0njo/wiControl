
//wiControl_4I4O_Ed01(0,0,0, 0,0,0);
wiControl_4I4O_Ed02(0,0,0, 0,0,0);

 module cylinder_outer(height,radius,fn){
   fudge = 1/cos(180/fn);
   cylinder(h=height,r=radius*fudge,$fn=fn);}
   
// frame to capture front plate or buttons
module wiControl_4I4O_Ed02(x, y, z, rx, ry, rz) {
 translate([x,y,z]) {
  rotate([rx,ry,rz]) {

   difference() {

    union() {

     difference() {
      // global face plate 71x71mm
//    translate([-24.0,-24.0,0.0]) {
//     cube([48.0,48.0,1.0]);
//    }
      translate([-35.5,-35.5,0.0]) {
       cube([71.0,71.0,2.0]);
      }

      // make edge around 
      translate([-34.0,-34.0,1.0]) {
       cube([68.0,68.0,2.0]);
      }   

     // cutout for NIKO insert
     translate([-21.0,-21.0,-1.0]) {
      cube([42.00,42.00,3.0]);
     }

    } // diff

   // inner lockers for NIKO insert around cutout 
   difference() {
    union() {
     translate([-22.0,21.0,1.0]) {
      cube([44.0,1.0,5.0]);
     }
     translate([-22.0,-22.0,1.0]) {
      cube([1.0,44.0,5.0]);
     }
     translate([-22.0,-22.0,1.0]) {
      cube([44.0,1.0,5.0]);
     }
     translate([21.0,-22.0,1.0]) {
      cube([1.0,44.0,5.0]);
     }
    } // union

    translate([-22.0,-3.5,4.4]) {
     cube([44.0,7.0,5.0]);
    }
    translate([-3.5,-22.0,4.4]) {
     cube([7.0,44.0,5.0]);
    }
   } // diff
    
  // wiControl housing
   difference() {

   intersection() {
    translate([-26.00,-26.0,1.0]) {
     cube([52.00,52.0,23.0]);
    }
    translate([0,0,1.0]) {
     cylinder(h=23.0, r=30.7, $fn=360);
    }
   } // intersect

   intersection() {
    translate([-25.00,-25.0,0.0]) {
     cube([50.00,50.0,25.0]);
    }
    translate([0,0,0.0]) {
     cylinder(h=25.0, r=29.7, $fn=360);
    }
   } // intersect
   
    // cross corner cutouts to save space
    translate([0.0,0.0,22.0]) {
     rotate([0,0,-45]) {
      translate([-32.0,-5.3,0.0]) {
       cube([64.0,10.6,6.0]);
      } // translate
     } // rot
    } // translate
    translate([0.0,0.0,22.0]) {
     rotate([0,0,45]) {
      translate([-32.0,-5.3,0.0]) {
       cube([64.0,10.6,6.0]);
      } // translate
     } // rot
    } // translate

   } // diff
  
  // mounting poles for wiControl
  difference() {
   union() {
     translate([12.97,-26.00,1.0]) {
      cube([2.0,3.0,21.0]);
     }
     translate([13.97,-21.59,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=21.0, r=2.54, $fn=360);
      }
     }
     translate([12.97,23.00,1.0]) {
      cube([2.0,3.0,21.0]);
     }
     translate([13.97,21.59,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=21.0, r=2.54, $fn=360);
      }
     }
   } // union 

   // keep room for NIKO insert
   translate([-21.0,-21.0,0.0]) {
    cube([42.0,42.0,12.0]);
   }

    // wiControl mounting pole holes 
    translate([13.97,-21.59,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=22.0, r=1.44, $fn=360);
      }
     }
    translate([13.97,21.59,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=22.0, r=1.44, $fn=360);
      }
     }
    } // diff

   // extra support for top/side slide holes
   translate([-35.00,-4.00,1.0]) {
    cube([9.0,8.0,1.0]);
   }
   translate([ 26.00,-4.00,1.0]) {
    cube([9.0,8.0,1.0]);
   }
   translate([-4.00,-35.00,1.0]) {
    cube([8.0,9.0,1.0]);
   }
   translate([-4.00,26.00,1.0]) {
    cube([8.0,9.0,1.0]);
   }

   // extra support for cross corner screw holes
   // bottom left
   translate([-31.25,-19.25,1.0]) {
    rotate([0,0,0]) {
     cylinder(h=1.0, r=3.0, $fn=360);
    }
   }
   translate([-35.25,-22.25,1.0]) {
    cube([4.0,6.0,1.0]);
   }
   // bottom right
   translate([31.25,-19.25,1.0]) {
    rotate([0,0,0]) {
     cylinder(h=1.0, r=3.0, $fn=360);
    }
   }
   translate([ 31.25,-22.25,1.0]) {
    cube([4.0,6.0,1.0]);
   }
   // top left
   translate([-31.25, 19.25,1.0]) {
    rotate([0,0,0]) {
     cylinder(h=1.0, r=3.0, $fn=360);
    }
   }
   translate([-35.25, 16.25,1.0]) {
    cube([4.0,6.0,1.0]);
   }
   // top right
   translate([ 31.25, 19.25,1.0]) {
    rotate([0,0,0]) {
     cylinder(h=1.0, r=3.0, $fn=360);
    }
   }
   translate([ 31.25, 16.25,1.0]) {
    cube([4.0,6.0,1.0]);
   }

   } // union 

      // cross corner screw holes
      // bottom left
      translate([-31.25,-19.25,-1.0]) {
       cylinder(h=4.0, r=1.75, $fn=360);
      }
      // bottom right
      translate([ 31.25,-19.25,-1.0]) {
       cylinder(h=4.0, r=1.75, $fn=360);
      }
      // top left
      translate([-31.25, 19.25,-1.0]) {
       cylinder(h=4.0, r=1.75, $fn=360);
      }
      // top right
      translate([ 31.25, 19.25,-1.0]) {
       cylinder(h=4.0, r=1.75, $fn=360);
      }


      // holes for 2 NIKO cover plate brackets
      
      // left top
      translate([-16.60, 23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-24.10, 14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-25.80, 21.80,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // right top
      translate([ 16.60, 23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 24.10, 14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 22.00, 25.60,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // left bottom
      translate([-16.60,-23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-24.10,-14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-22.00,-25.60,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // right bottom
      translate([ 16.60,-23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 24.10,-14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 25.80,-21.80,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }
      
    // attaching slides left and right
      
    // large holes in edge
    rotate([0,0,-15]) {
     union() {
      translate([30.5,0.0,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=2.0, r=3.5, $fn=360);
       }
      }
      translate([0.0,-30.5,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=2.0, r=3.5, $fn=360);
       }
      }
      translate([-30.5,0.0,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=2.0, r=3.5, $fn=360);
       }
      }
      translate([0.0,30.5,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=2.0, r=3.5, $fn=360);
       }
      }
     } // union
    } // rotate
    
   intersection() {
    difference() {
     translate([0.0,0.0,-1.0]) {
      rotate([0,0,0]) {
       cylinder(h=4.0, r=32.25, $fn=360);
      }
     }
     translate([0.0,0.0,-1.0]) {
      rotate([0,0,0]) {
       cylinder(h=4.0, r=28.75, $fn=360);
      }
     }
    } // diff

    union() {
        
     polyhedron(points=[[cos(-15)*40, sin(-15)*40, 0],
                        [cos( 15)*40, sin( 15)*40, 0],
                        [0,0,0],
                        [cos(-15)*40, sin(-15)*40, 3],
                        [cos( 15)*40, sin( 15)*40, 3],
                        [0,0,3]],
      faces=[[0,1,2],[3,4,1,0],[5,4,3],[4,5,2,1],[5,3,0,2]]);

     polyhedron(points=[[cos( 75)*40, sin( 75)*40, 0],
                        [cos(105)*40, sin(105)*40, 0],
                        [0,0,0],
                        [cos( 75)*40, sin( 75)*40, 3],
                        [cos(105)*40, sin(105)*40, 3],
                        [0,0,3]],
      faces=[[0,1,2],[3,4,1,0],[5,4,3],[4,5,2,1],[5,3,0,2]]);

     polyhedron(points=[[cos(165)*40, sin(165)*40, 0],
                        [cos(195)*40, sin(195)*40, 0],
                        [0,0,0],
                        [cos(165)*40, sin(165)*40, 3],
                        [cos(195)*40, sin(195)*40, 3],
                        [0,0,3]],
      faces=[[0,1,2],[3,4,1,0],[5,4,3],[4,5,2,1],[5,3,0,2]]);

     polyhedron(points=[[cos(255)*40, sin(255)*40, 0],
                        [cos(285)*40, sin(285)*40, 0],
                        [0,0,0],
                        [cos(255)*40, sin(255)*40, 3],
                        [cos(285)*40, sin(285)*40, 3],
                        [0,0,3]],
      faces=[[0,1,2],[3,4,1,0],[5,4,3],[4,5,2,1],[5,3,0,2]]);

    } // union
    } // intersect
    
    // small holes in edge
    rotate([0,0,15]) {
     union() {
      translate([30.5,0.0,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=4.0, r=1.75, $fn=360);
       }
      }
      translate([0.0,-30.5,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=4.0, r=1.75, $fn=360);
       }
      }
      translate([-30.5,0.0,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=4.0, r=1.75, $fn=360);
       }
      }
      translate([0.0,30.5,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=4.0, r=1.75, $fn=360);
       }
      }
     } // union
    } // rotate

   } // diff

  } // rotate
 } // translate
} // wiControl_4I4O_Ed02


// 1.95" x 1.95" module cover plate 
module wiControl_4I4O_Ed01(x, y, z, rx, ry, rz) {
 translate([x,y,z]) {
  rotate([rx,ry,rz]) {
   
   difference() {

    union() {
     translate([-35.5,-4.0,1.0]) {
      cube([10.0,8.0,1.0]);
     }
     translate([ 25.5,-4.0,1.0]) {
      cube([10.0,8.0,1.0]);
     }
     translate([-3.0,-35.5,1.0]) {
      cube([6.0,10.0,1.0]);
     }
     translate([-3.0, 25.5,1.0]) {
      cube([6.0,10.0,1.0]);
     }

     difference() {
      // bottom flange
      translate([-35.5,-35.5,0.0]) {
       cube([71.0,71.0,2.0]);
      }

      // make edge around
      translate([-34.0,-34.0,1.0]) {
       cube([68.0,68.0,2.0]);
      }
      
      // cutouts to break of edge
      translate([-27.765,-27.765,0.0]) {
       cube([55.53,1.0,0.25]);
      }
      translate([-27.765,-27.765,0.0]) {
       cube([1.0,55.53,0.25]);
      }
      translate([-27.765, 26.765,0.0]) {
       cube([55.53,1.0,0.25]);
      }
      translate([ 26.765,-27.765,0.0]) {
       cube([1.0,55.53,0.25]);
      }

      translate([-27.765,-27.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }
      translate([-27.765,-27.765,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([-5.0,-27.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }

      translate([-27.765, 26.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }
      translate([-27.765, 17.765,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([-27.765,-5.0,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([ 17.765,-27.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }
      translate([ 26.765,-27.765,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([ 26.765,-5.0,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([ 17.765, 26.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }
      translate([ 26.765, 17.765,-1.0]) {
       cube([1.0,10.0,4.0]);
      }

      translate([-5.0, 26.765,-1.0]) {
       cube([10.0,1.0,4.0]);
      }

     } // diff

     // bottom
     difference() {
      translate([-26.765,-26.765,1.0]) {
       cube([53.53,53.53,1.0]);
      }
      translate([-23.765,-23.765,1.0]) {
       cube([47.53,47.53,2.0]);
      }
     } // diff
     

     // left protection tab
     translate([-26.765,-26.765,2.0]) {
      cube([1.0,53.53,11.0]);
     }
     // bottom protection tab
     translate([-26.765,-26.765,2.0]) {
      cube([53.53,1.0,11.0]);
     }
     // top protection tab
     translate([-26.765,25.765,2.0]) {
      cube([53.53,1.0,11.0]);
     }
     // right protection tab
     translate([25.765,-26.765,2.0]) {
      cube([1.0,53.53,11.0]);
     }
     
     // resting tabs
     translate([-26.765,-19.415,1.0]) {
      cube([4.54,2.0,10.0]);
     }
     translate([-26.765,17.415,1.0]) {
      cube([4.54,2.0,10.0]);
     }
     
     // mounting poles
     translate([12.335,-26.765,1.0]) {
      cube([2.0,3.0,10.0]);
     }
     translate([13.335,-22.225,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=10.0, r=2.54, $fn=360);
      }
     }
     translate([12.335,23.765,1.0]) {
      cube([2.0,3.0,10.0]);
     }
     translate([13.335,22.225,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=10.0, r=2.54, $fn=360);
      }
     }
    } // union

      // holes for 2 NIKO cover plate brackets
      
      // left top
      translate([-16.60, 23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-24.10, 14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-25.80, 21.80,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // right top
      translate([ 16.60, 23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 24.10, 14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 22.00, 25.60,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // left bottom
      translate([-16.60,-23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-24.10,-14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([-22.00,-25.60,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

      // right bottom
      translate([ 16.60,-23.20,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 24.10,-14.00,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.2, $fn=360); // 2mm
       }
      }
      translate([ 25.80,-21.80,-1.0]) {
       rotate([0,0,0]) {
        cylinder(h=3.0, r=1.4, $fn=360); // 2.2mm
       }
      }

    // cutouts to mount in wallbox
    translate([-28.015,0.0,-1.0]) {
     rotate([0,0,0]) {
      cylinder(h=15.0, r=1.75, $fn=360);
     }
    }
    translate([-28.015,0.0,0.0]) {
     rotate([0,0,0]) {
      cylinder(h=1.0, r1=3.0, r2=1.75, $fn=360);
     }
    }
    translate([28.015,0.0,-1.0]) {
     rotate([0,0,0]) {
      cylinder(h=15.0, r=1.75, $fn=360);
     }
    }
    translate([28.015,0.0,0.0]) {
     rotate([0,0,0]) {
      cylinder(h=1.0, r1=3.0, r2=1.75, $fn=360);
     }
    }

    // hole for led
    translate([19.685,-19.685,-2.0]) {
      rotate([0,0,0]) {
       cylinder(h=14.0, r=2.54, $fn=360);
      }
     }

    // wiControl mounting holes 
    translate([13.335,-22.225,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=13.0, r=1.44, $fn=360);
      }
     }
    translate([13.335,22.225,1.0]) {
      rotate([0,0,0]) {
       cylinder(h=13.0, r=1.44, $fn=360);
      }
     }

   } // diff 
   
  } // rotate
 } // translate
} // wiControl_4I4O_Ed01

