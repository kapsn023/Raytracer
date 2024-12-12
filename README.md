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

### Textures
Textures work in a similar way to material colors in the sense that all spheres
and faces are assigned the last declared texture. Therefore, one must declare all
faces and spheres without textures before any are declared with textures.
Those objects still need to have material colors, as the texture only overrides the 
ambient color of those objects. Textures are declared as follows:
```
texture texture.ppm
```
The texture must be a .ppm file. An error will result from an other file format.

### Spheres
Spheres are defined as follows:
```
sphere x y z r
```
The 'x y z' represents the location of the center of the sphere. 'r' represents the
radius of the sphere. It will take on the last defined material color and texture as 
its own.

### Faces
Faces are the most complicated to input. They require at least two distinct steps.
Firstly, one must define the vertices that are to make up the triangle. They
are defined as follows:
```
v x y z
```
The 'x y z' simply refers to the vertex's location in space. All vertices that are used
for a face must be defined before the face is. 

The next step is optional. It is required only if the face is to be textured. It looks as 
follows:
```
vt x y
```
This simply represents a vertex in the texture coordinate system.

Next, one can define a face with previously defined vertices. That looks as follows if no 
texture is to be used:
```
f v1 v2 v3
```
and as follows if the face will have a texture:
```
f v1/vt1 v2/vt2 v3/vt3
```
All of those variables represent the previously defined vertices, starting at 1 and going
up from there.

Below is an example of two faces being declared with and without textures, respectively.
```
mtlcolor 1 1 1 1 1 1 0.2 0.8 0 20 1 0
texture earthtexture.ppm
v 10 0 5
v -10 0 5
v -10 0 25
v 10 0 25

vt 0 1
vt 1 1
vt 1 0
vt 0 0 

f 1/1 2/2 3/3
f 1/1 3/3 4/4
```
```
mtlcolor 1 1 1 1 1 1 0.2 0.8 0 20 1 0
v 10 0 5
v -10 0 5
v -10 0 25
v 10 0 25

f 1 2 3
f 1 3 4
```
