# Ray Tracer Instructions
The use of the ray tracer is not particularly complicated, but it is also not the most intuitive. 
Thus, I feel compelled to leave these instructions for ease of use.

### Compiling and Makefile Use
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

### Running the Code
Once the code has been compiled, the executable can be run with the following command
(assuming the current directory is still the main directory):
```
./build/ray_tracer input.txt
```
The 'input.txt' is just a stand-in for any properly formatted input file. If everything
has been done correctly, a file called 'input.ppm' should have been created after a
couple of seconds and it should be identical to 'baseline.ppm'.

### Creating the Input File
