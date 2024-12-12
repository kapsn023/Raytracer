# Ray Tracer Instructions
The use of the ray tracer is not particularly complicated, but it is also not the most intuitive. 
Thus, I feel compelled to leave these instructions for ease of use.

## Compiling and Makefile Use
For the convience of the user, I have add a standard Makefile to this project. 
In order to compile the source files, one must simply use the following command:
```
make
```
This compiles all of the code in the 'src' folder and builds an executable in a new folder
called 'build'. At this point, the code is ready to be run. The executable file will be
called 'ray_tracer'.

To clean up the files and remove the 'build' folder along with the executables, one must
use the following command:
```
make clean
```
This will delete the 'build' folder with all of the executable files inside.

## Running the Code
Once the code has been compiled, the executable can be run with the following command
(assuming the current directory is still the main directory):
```
./build/ray_tracer input.txt
```
The 'input.txt' is just a stand-in for any properly formatted input file. If everything
has been done correctly, a file called 'input.ppm' should have been created after a
couple of seconds and it should be identical to 'baseline.ppm'.

## Creating the Input File
The ray tracer is looking for a specifically formatted input text file. It should be
generally assumed that the number values should lie on the range of 0.0 to 1.0.

### Mandatory Settings
Six options must be explicitly defined. They are as follows:
- Camera Location
- View Direction
- Up Direction
- Horizontal Field of View
- The Size of the Output Image
- The Background Color

With these six defined, the ray tracer is able to produce an image. The formatting for
these are as follows (with 'x y z' and 'r g b a' in the place of decimal numbers):
```
eye x y z
viewdir x y z
updir x y z
hfov x
imsize x y
bkgcolor r g b a
```

The example from the provided input file looks as follows:
```
eye 0 5 0
viewdir 0 0 1
updir 0 1 0
hfov 45
imsize 1080 1080
bkgcolor 0.5 0.7 0.9 1
```

### Lights
At this point, the environment is ready to be populated with objects. The one with the
simplest formatting is lights, as they are not reliant on other objects. To do this,
uses a line similar to the one below.
```
light x y z w r g b
```
The 'w' dictates whether or not this light acts as a point or directional light. If 'w'
is a 0, than the light is a directional light. In those instances, then 'x y z' 
represents the direction from which the light is coming. If the 'w' is instead a 1,
then the light is a point light and 'x y z' then represents the location of the light.

The 'r g b' always represents the color of the light. Here is the example used in the 
provided input file (which represents a white directional light pointing straight down):
```
light 0 -1 0 0 1 1 1
```

### Material Colors
In order to place faces and spheres into the environment, one must first define a
material color. Spheres and faces will take on the properties of the last material
color to be defined. If none have yet been defined, an error results. They can be
defined with the following formatting:
```
mtlcolor dr dg db sr sg sb ka kd ks n a eta
```
The first three numbers, 'dr dg db', are representing the diffuse color of the 
material, which is also used to define the ambient color of the material.
For 'sr sg sb', the specular color is being represented. 'ka kd ks'
are weights that control the surface's ambient, diffuse, and specular reflectively,
respectively. 

Below is a picture of spheres with varying ka values, going from 0.2 to 0.8.

![](/images/ka_demonstration.png)

Below is a picture of spheres with varying kd values, going from 0.2 to 0.8.

![](/images/kd_demonstration.png)

Below is a picture of spheres with varying ks values, going from 0.2 to 0.8.

![](/images/ks_demonstration.png)

The 'n' controls the rate of falloff of the specular highlight and its
intensity. It can be seen below, with values of 2, 6, 10, 50, and 100,
respectively.

![](/images/n_demonstration.png)

The 'eta' (as in the Greek character) refers to the index of refraction and 
'a' refers to the material's opacity; both of which impact how the material
reflects and bends lights.
